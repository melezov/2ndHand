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

DWORD sum0, sum1, ccc;

void InitIS_PLASMA( IS_FRAME *f, DWORD seed, float plasmaChaos );

DWORD pp = 0;
IS_FRAME *b = 0;

LRESULT TestParse( UINT m_uMsg, WPARAM m_wParam, LPARAM m_lParam )
{
    if ( m_uMsg == WM_PAINT )
    {
        PAINTSTRUCT x_PS;
        HDC hDC = BeginPaint( gm_hWnd, &x_PS );

        RECT cRect;
        GetClientRect( gm_hWnd, &cRect );
        {
            if ( b && ( ( b->_x != cRect.right ) || ( b->_y != cRect.bottom ) ) )
            {
                KillIS_FRAME( b );
                b = 0;
            }

            if ( !b )
            {
                if ( pp == 0 ) pp = GetTickCount();
                b = MakeIS_FRAME( cRect.right, cRect.bottom );
                InitIS_PLASMA( b, pp, 10.0f );            
            }

            if ( cnt == -1 )
            {
                DWORD aa;

                for ( aa = 1; aa <= 16; aa ++ )
                {
                    IS_HEADER h = { IDB_POINTER, aa, 80, 60, 2.0f, 5.0f, 3.0f, 1234, 0 };

                    if ( !( ff[ aa ] = MakeIS_FACTORY( &h ) ) )
                    {
                        MsgErr( 0x1000, MB_ICONERROR );
                    }
                }

                cnt = 3;
                f = ff[ cnt ];
            }

            if ( b )
            {
                TCHAR aaa[ 500 ];
                COAL *koment;
                LARGE_INTEGER freq, t0, t1;//, t2;
                
                IS_FRAME *r1 = 0;//, *r2 = 0;
                float tt = GetTickCount() / 500.0f;
                
                switch ( f->head.aaFactor )
                {
                    case  1: koment = TEXT( "(gadno... bez AA)" ); break;
                    case  2: koment = TEXT( "(bolje)" ); break;
                    case  3: koment = TEXT( "(optimalno)" ); break;
                    case  4: koment = TEXT( "(XHQ)" ); break;
                    case  5: koment = TEXT( "(320 kbps VBR mp3)" ); break;
                    case 16: koment = TEXT( "(256 sampleova za 1 pixel :)" ); break;
                    
                    default: koment = TEXT( "(pretjerivanje)" );
                }

     //           BitBlt( b->hDC, 0, 0, cRect.right, cRect.bottom, 0, 0, 0, WHITENESS );

                QueryPerformanceFrequency(&freq);
                QueryPerformanceCounter(&t0);
                r1 = RendIS_FRAME( f, tt );
                QueryPerformanceCounter(&t1);
/*                r2 = Rend2S_FRAME( f, tt );
                QueryPerformanceCounter(&t2);
*/              
                if ( r1 )
                {
/*                    int x, y;

                    if ( ( r1->_x != r2->_x ) || ( r1->_y != r2->_y ) )
                    if ( ( r1->_ix != r2->_ix ) || ( r1->_iy != r2->_iy ) )
                    {
                        __asm int 3
                        ExitProcess( -1 );
                    }
                    
                    for ( x = 0; x < r1->_x; x ++ )
                    for ( y = 0; y < r2->_y; y ++ )
                    {
                        DWORD c1 = _PX( r1, x, y );
                        DWORD c2 = _PX( r2, x, y );

                        if ( c1 != c2 )
                        {
                            __asm int 3
                            ExitProcess( -2 );
                        }
                    }
*/
                    Move2ndHand(f->hwnd, r1, &lastPt );                    

                    ccc ++;
                    sum0 += (DWORD) ( ( ( t1.QuadPart - t0.QuadPart ) * 1000000 ) / freq.QuadPart );
//                    sum1 += (DWORD) ( ( ( t2.QuadPart - t1.QuadPart ) * 1000000 ) / freq.QuadPart );
                   
                    wsprintf( aaa, TEXT( "\n\n\n\nFeatures:\n\n" )
                        TEXT( "Nema jebenog #@$@$ MSVCRXX.dll kretenizma!\n" )
                        TEXT( "Single .exe fajl, nema prateæeg DLLa/importa/exporta!\n" )
                        TEXT( "MMX enhanced! w00ty! ;)\n" )
                        TEXT( "Svi floating point op. u assemblyju!\n")
                        TEXT( "Plazma fraktal je neovisan o rezoluciji!\n" )
                        TEXT( "Hooka low level missa + tipkalo!\n" )
                        TEXT( "ANSI / UNICODE compatible!\n" )
                        TEXT( "\nAntialiasing: %dx%d: %d us\n%s\n" )
                        TEXT( "\n\n... release any key to swap ..." ), f->head.aaFactor, f->head.aaFactor, sum0 / ccc, koment );
                    

                    {
                        RECT dRect;
                        MemZero( sizeof( RECT ) / 4, &dRect );

                        DrawText( b->hDC, aaa, lstrlen( aaa ), &dRect, DT_LEFT | DT_CENTER | DT_CALCRECT );
                        
                        OffsetRect( &dRect, ( cRect.right - dRect.right ) / 2, ( cRect.bottom - dRect.bottom ) / 2 );                        
                        BitBlt( b->hDC, dRect.left, dRect.top, dRect.right - dRect.left + 1, dRect.bottom - dRect.top - 1, 0, 0, 0, WHITENESS );
                        InflateRect( &dRect, 2, 2 );
                        DrawEdge( b->hDC, &dRect, EDGE_SUNKEN, BF_RECT );
                        InflateRect( &dRect, 2, 2 );
                        DrawEdge( b->hDC, &dRect, EDGE_RAISED, BF_RECT );
                        
                        DrawText( b->hDC, aaa, lstrlen( aaa ), &dRect, DT_LEFT | DT_CENTER );
                    }

                    KillIS_FRAME( r1 );
//                    KillIS_FRAME( r2 );
                } 
                
                BitBlt( hDC, 0, 0, cRect.right, cRect.bottom, b->hDC, 0, 0, SRCCOPY );                
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
            if ( cnt > 16 ) cnt = 1;
            f = ff[ cnt ];
            ccc = 0;
            sum0 = 0;
            sum1 = 0;
        }

        InvalidateRect( gm_hWnd, 0, 0 );
    }

    return 1;
}
