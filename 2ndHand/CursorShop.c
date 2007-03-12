#include "CursorShop.h"

/*
BOOL OpenCursorHBM( STRING s_OFN )
{
	OPENFILENAME x_OFN = { 0 };

	x_OFN.lStructSize = sizeof( OPENFILENAME );
	x_OFN.hwndOwner = gm_hWnd;
    x_OFN.lpstrFilter = TEXT( "Bitmap files (*.bmp)\0*.bmp\0All Files\0*.*\0\0" );
	x_OFN.lpstrFile = s_OFN;
	x_OFN.nMaxFile = MAX_PATH;
    x_OFN.lpstrInitialDir = 0; //
	x_OFN.lpstrTitle = TEXT( "Open Cursor ..." );
	x_OFN.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_LONGNAMES | OFN_HIDEREADONLY;
	x_OFN.FlagsEx = OFN_EX_NOPLACESBAR;

	if ( !GetOpenFileName( &x_OFN ) ) return ERROR_CURSORSHOP( 7 );

	return ERROR_SUCCESS;
}
*/

DWORD RenderThread( void *f_xParam )
{
	CS_FACTORY *f = (CS_FACTORY *) f_xParam;
	HANDLE fM = f->mutex;

	WORD rotStep, schedule, renderedCount;

	do
	{
		schedule = f->head.rotSteps;

		WaitForSingleObject( f->mutex, INFINITE );

		for ( rotStep = 0; rotStep < f->head.rotSteps; rotStep ++ )
		{
			BYTE flag = f->rots[ rotStep ].flag;

			if ( flag == CS_FLAG_IDLE )
			{
				schedule = rotStep;
			}

			else if ( flag == CS_FLAG_PRIORITY )
			{
//				outtyText( "GOT PRIORITY", rotStep );
				schedule = rotStep;
				break;
			}
		}

		if ( schedule != f->head.rotSteps )
		{
			BOOL iRender;

//			outtyText( "Gonnarun", schedule );
//			outtyText( "becauseF", f->rots[ schedule ].flag );
			f->rots[ schedule ].flag = CS_FLAG_INPROCESS;
//			outtyText( "now IS", f->rots[ schedule ].flag );
			ReleaseMutex( fM );

			iRender = RenderCS_Frame( f, schedule );

//			if ( iRender ) outtyText( "BROKERRRRRRRERRRERENNN!!!!", schedule );
			WaitForSingleObject( f->mutex, INFINITE );
			f->rots[ schedule ].flag = iRender ? CS_FLAG_IDLE : CS_FLAG_RENDERED;
		}

		renderedCount = 0;

		for ( rotStep = 0; rotStep < f->head.rotSteps; rotStep ++ )
		{
			if ( f->rots[ rotStep ].flag == CS_FLAG_RENDERED ) renderedCount ++;
		}
		ReleaseMutex( fM );
	}
	while( renderedCount < rotStep );

	return ERROR_SUCCESS;
}

void RenderCS_Factory( CS_FACTORY *f )
{
	SYSTEM_INFO x_SI;
	GetSystemInfo( &x_SI );

	while( x_SI.dwNumberOfProcessors -- != 0 )
	{
		HANDLE newThread = CreateThread( 0, 0, (LPTHREAD_START_ROUTINE) RenderThread, f, 0, 0 );
		SetThreadPriority( newThread, THREAD_PRIORITY_BELOW_NORMAL );
		CloseHandle( newThread );
	}
}
