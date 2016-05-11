#include "TestProc.h"
#include "FramServ.h"
#include "PlasRend.h"
#include "FramRend.h"

TCHAR kBuf[ MAX_STRING ];
TCHAR rBuf[ MAX_STRING ];

POINT lastPt;

DWORD cnt = -1;
IS_FACTORY *ff[ 17 ];
IS_FACTORY *f;

DWORD sum0, ccc;

LRESULT TestParse( UINT m_uMsg, WPARAM m_wParam, LPARAM m_lParam )
{
    if ( m_uMsg == WM_PAINT )
    {
        PAINTSTRUCT x_PS;
        HDC hDC = BeginPaint( gm_hWnd, &x_PS );

        RECT cRect;
        GetClientRect( gm_hWnd, &cRect );
        {
            IS_FRAME *b = MakeIS_FRAME( cRect.right, cRect.bottom );

            if ( cnt == -1 )
            {
                DWORD aa;

                for ( aa = 2; aa <= 16; aa ++ )
                {
                    IS_HEADER h = { IDB_PICKER, aa, 80, 60, 3.0f, 5.0f, 3.0f, 1234, 0 };

                    if ( !( ff[ aa ] = MakeIS_FACTORY( &h ) ) )
                    {
                        MsgErr( 0x1000, MB_ICONERROR );
                    }
                }

                cnt = 2;
                f = ff[ cnt ];
            }

            if ( b )
            {
                TCHAR aaa[ MAX_STRING ];
                LARGE_INTEGER freq, t0, t1;
                
                IS_FRAME *r;
                float tt = GetTickCount() / 2000.0f;
                
                BitBlt( b->hDC, 0, 0, cRect.right, cRect.bottom, 0, 0, 0, WHITENESS );

                QueryPerformanceFrequency(&freq);

                QueryPerformanceCounter(&t0);
                r = RendIS_FRAME( f, tt );
                QueryPerformanceCounter(&t1);
              
                if ( r )
                {
                    Move2ndHand( f->hwnd, r, &lastPt );
                    ccc ++;
                    sum0 += (DWORD) ( ( ( t1.QuadPart - t0.QuadPart ) * 1000000 ) / freq.QuadPart );
                   
                    wsprintf( aaa, TEXT( "\n\n\n\nFeatures:\n\nNema jebenog #@$@$ MSVCRXX.dll kretenizma!\n" )
                        TEXT( "Single .exe fajl, nema prateæeg DLLa/importa/exporta!\nMMX enhanced! w00ty! ;)\n" )
                        TEXT( "ANSI / UNICODE compatible!\n\nAntialiasing: %dx%d: %d us\n\n\n... release any key to swap ..." ), f->head.aaFactor, f->head.aaFactor, sum0 / ccc );
                    DrawText( b->hDC, aaa, lstrlen( aaa ), &cRect, DT_LEFT | DT_CENTER );

                    KillIS_FRAME( r );
                } 
                
                BitBlt( hDC, 0, 0, cRect.right, cRect.bottom, b->hDC, 0, 0, SRCCOPY );                
                KillIS_FRAME( b );
            }
        }
        EndPaint( gm_hWnd, &x_PS );
    }
    
    else if ( m_uMsg == WM_RATHOOK )
    {
        wsprintf( rBuf, TEXT( "   X:%04X, Y:%04X, A:%04X, W:%04X   " ), LOWORD( m_lParam ), HIWORD( m_lParam ), LOWORD( m_wParam ), HIWORD( m_wParam ) );

        lastPt.x = (short)LOWORD( m_lParam );
        lastPt.y = (short)HIWORD( m_lParam );
    
        InvalidateRect( gm_hWnd, 0, 0 );
    }
    
    else if ( m_uMsg == WM_KEYHOOK )
    {
        wsprintf( kBuf, TEXT( "   V:%04X, S:%04X, A:%04X, X:%04X   " ), LOWORD( m_lParam ), HIWORD( m_lParam ), LOWORD( m_wParam ), HIWORD( m_wParam ) );

        if ( LOWORD( m_wParam ) == WM_KEYUP )
        if ( cnt != -1 )
        {
            cnt ++;
            if ( cnt > 16 ) cnt = 2;            
            f = ff[ cnt ];
            ccc = 0;
            sum0 = 0;
        }

        InvalidateRect( gm_hWnd, 0, 0 );
    }

    return 1;
}
