#include "CursorShop.h"
//
///*
//DWORD RenderThread( void *f_xParam )
//{
//	CS_FACTORY *f = (CS_FACTORY *) f_xParam;
//	HANDLE fM = f->mutex;
//
//	WORD rotStep, schedule, renderedCount;
//
//	do
//	{
//		schedule = f->head.rotSteps;
//
//		WaitForSingleObject( f->mutex, INFINITE );
//
//		for ( rotStep = 0; rotStep < f->head.rotSteps; rotStep ++ )
//		{
//			BYTE flag = f->rots[ rotStep ].flag;
//
//			if ( flag == CS_FLAG_IDLE )
//			{
//				schedule = rotStep;
//			}
//
//			else if ( flag == CS_FLAG_PRIORITY )
//			{
////				outtyText( "GOT PRIORITY", rotStep );
//				schedule = rotStep;
//				break;
//			}
//		}
//
//		if ( schedule != f->head.rotSteps )
//		{
//			BOOL iRender;
//
////			outtyText( "Gonnarun", schedule );
////			outtyText( "becauseF", f->rots[ schedule ].flag );
//			f->rots[ schedule ].flag = CS_FLAG_INPROCESS;
////			outtyText( "now IS", f->rots[ schedule ].flag );
//			ReleaseMutex( fM );
//
//			iRender = RenderCS_Frame( f, schedule );
//
////			if ( iRender ) outtyText( "BROKERRRRRRRERRRERENNN!!!!", schedule );
//			WaitForSingleObject( f->mutex, INFINITE );
//			f->rots[ schedule ].flag = iRender ? CS_FLAG_IDLE : CS_FLAG_RENDERED;
//		}
//
//		renderedCount = 0;
//
//		for ( rotStep = 0; rotStep < f->head.rotSteps; rotStep ++ )
//		{
//			if ( f->rots[ rotStep ].flag == CS_FLAG_RENDERED ) renderedCount ++;
//		}
//		ReleaseMutex( fM );
//	}
//	while( renderedCount < rotStep );
//
//	return ERROR_SUCCESS;
//}
//
//void RenderCS_Factory( CS_FACTORY *f )
//{
//	SYSTEM_INFO x_SI;
//	GetSystemInfo( &x_SI );
//
//	while( x_SI.dwNumberOfProcessors -- != 0 )
//	{
//		HANDLE newThread = CreateThread( 0, 0, (LPTHREAD_START_ROUTINE) RenderThread, f, 0, 0 );
//		SetThreadPriority( newThread, THREAD_PRIORITY_BELOW_NORMAL );
//		CloseHandle( newThread );
//	}
//}
//
//*/
