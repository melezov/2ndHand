#include "CursorShop.h"

CS_FACTORY *r;

void TestCenter( int m_uMsg )
{
    if ( m_uMsg == WM_CREATE )
    {
        CS_HEADER h;

        h.aaFactor = 2;
        h.borderWidth = 3.25f;
        h.destSize.cx = 80;
        h.destSize.cy = 60;
        h.plasmaChaos = 4.0f;
        h.randSeed = GetTickCount();
        wsprintf( h.resId, TEXT( "..\\Bitmaps\\0x20 - Picker.bmp" ) );
        h.shadowDist = 3.0f;

        r = MakeCS_Factory( &h );
        if ( !r ) ExitProcess( -1 );
    }

    else if ( m_uMsg == WM_RATHOOK )
    {
        POINT cPos;
        GetCursorPos( &cPos );

        PostCS_Frame( r, &cPos );
    }

    else if ( m_uMsg == WM_KEYDOWN )
    {
        DestroyQueue( r );
    }
}

/*        CS_FRAME *f;
        POINT cPos;

        static BOOL aoe = 0;
        static POINT t_pDst;
        float newAngle;

        static float oldAngle = 0;
        static POINT oPos;

        float pi2;

        GetCursorPos( &cPos );

        __asm
        {
            finit

            mov     eax, oPos.x
            mov     ecx, oPos.y
            sub     eax, cPos.x
            sub     ecx, cPos.y

            fldpi
            fld1
            fadd    st, st
            fmulp   st(1), st
            fstp    pi2
            fwait

            or      eax, eax
            jnz     FPATAN_GO

            or      ecx, ecx
            jnz     FPATAN_GO

            mov     eax, oldAngle
            mov     newAngle, eax

            jmp     FPATAN_EX

FPATAN_GO:  push    eax
            push    ecx

            fild     [ esp ]
            fild     [ esp + 4 ]
            fpatan

            fstp    [ esp + 4 ]
            fwait
            pop     eax
            pop     newAngle

FPATAN_EX:
        }

        while( abz( oldAngle - newAngle - pi2 ) < abz( oldAngle - newAngle ) ) oldAngle -= pi2;
        while( abz( oldAngle - newAngle + pi2 ) < abz( oldAngle - newAngle ) ) oldAngle += pi2;

        oldAngle = ( oldAngle * ( CS_ANGLE_APPROACH - 1 ) + newAngle ) / CS_ANGLE_APPROACH;

        t_pDst.x += cPos.x - oPos.x;
        t_pDst.y += cPos.y - oPos.y;

        SetWindowPos( gc_hWnd, HWND_TOPMOST, t_pDst.x, t_pDst.y, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW );

        f = RendCS_Frame( r, oldAngle );

        if ( f )
        {
    	    BLENDFUNCTION blenda = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
            POINT pNew = { cPos.x, cPos.y };

		    POINT t_pSrc = { 0, 0 };
            SIZE  t_pSiz = { f->_x, f->_y };

            t_pDst.x = pNew.x - f->_ix;
            t_pDst.y = pNew.y - f->_iy;

            UpdateLayeredWindow( gc_hWnd, 0, &t_pDst, &t_pSiz, f->hDC, &t_pSrc, 0, &blenda, ULW_ALPHA );
            SetWindowPos( gc_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW );

            KillCS_Frame( f );
        }

        oPos.x = cPos.x;
        oPos.y = cPos.y;
    }
*/


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
