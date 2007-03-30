#include "CursorShop.h"

BOOL MakeCS_Frame( CS_FRAME *f, int f_iX, int f_iY )
{
	MemZero( sizeof( CS_FRAME ) / 4, f );

	f->bih.biSize = sizeof( BITMAPINFOHEADER );
	f->bih.biPlanes = 1;
	f->bih.biBitCount = 32;
	f->bih.biSizeImage = f_iX * f_iY << 2;

	f->_x =   ( f->_rl = f_iX );
	f->_y = - ( f->_rt = f_iY );

    f->hBM = CreateDIBSection( 0, (BITMAPINFO *) &f->bih, 0, &f->dPx, 0, 0 );
    if ( !f->hBM ) return ERROR_CURSORSHOP( 0 );

    f->_y = f_iY;

    f->hDC = CreateCompatibleDC( 0 );
    if ( !f->hDC ) return ERROR_CURSORSHOP( 1 );

    f->hGO = SelectObject( f->hDC, f->hBM );
    if ( !f->hGO ) return ERROR_CURSORSHOP( 2 );

    return ERROR_SUCCESS;
}

BOOL KillCS_Frame( CS_FRAME *f )
{
    BOOL iKill;

    iKill = (BOOL) SelectObject( f->hDC, f->hGO );
    if ( !iKill ) return ERROR_CURSORSHOP( 10 );

    iKill = DeleteDC( f->hDC );
    if ( !iKill ) return ERROR_CURSORSHOP( 11 );

    iKill = DeleteObject( f->hBM );
    if ( !iKill ) return ERROR_CURSORSHOP( 12 );

	MemZero( sizeof( CS_FRAME ) / 4, f );
    return ERROR_SUCCESS;
}

void TestCS_Frame( CS_FRAME *f, int t_iX, int t_iY )
{
	if ( f->_rr < t_iX ) f->_rr = t_iX;
	if ( f->_rb < t_iY ) f->_rb = t_iY;
	if ( f->_rl > t_iX ) f->_rl = t_iX;
	if ( f->_rt > t_iY ) f->_rt = t_iY;
}

void PlasmaCS_Frame( CS_FRAME *f, CS_HEADER *h );

void TestCenter( int m_uMsg )
{
    if ( m_uMsg == WM_PAINT )
    {
        RECT cRect;
        PAINTSTRUCT x_PS;
        HDC tDC = BeginPaint( gm_hWnd, &x_PS );
        GetClientRect( gm_hWnd, &cRect );
        {
            CS_FRAME f;
            MakeCS_Frame( &f, cRect.right, cRect.bottom );
            {
                TCHAR buffy[ 1000 ];
                LARGE_INTEGER freq, t0, t1;
                int c;
                QueryPerformanceFrequency(&freq);
                SetPriorityClass( GetCurrentProcess(), HIGH_PRIORITY_CLASS );

                MemZero( f._x * f._y, f.dPx );
                QueryPerformanceCounter(&t0);
                for ( c = 0; c < 1; c ++ )
                {
                    CS_HEADER h;

                    h.plasmaChaos = 5.0f;
                    h.randSeed = 1;

                    PlasmaCS_Frame( &f, &h );
                }
                QueryPerformanceCounter(&t1);
                wsprintf( buffy, TEXT( "P2: %d" ), ((t1.QuadPart-t0.QuadPart)*1000000)/freq.QuadPart );
                BitBlt( tDC, 0, 0, cRect.right, cRect.bottom, f.hDC, 0, 0, SRCCOPY );
                TextOut( tDC, 0, 0, buffy, lstrlen( buffy ) );

            }
            KillCS_Frame( &f );
        }
        EndPaint( gm_hWnd, &x_PS );
    }

    else if ( m_uMsg == WM_SIZE )
    {
        InvalidateRect( gm_hWnd, 0, 0 );
    }
}
