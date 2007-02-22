#include "MainGUI.h"

HINSTANCE w_hInst;
HWND w_hWnd;

CF_WS *aa;

LRESULT CALLBACK m_pProc( HWND m_hWnd, UINT m_uMsg, WPARAM m_wParam, LPARAM m_lParam )
{
	if ( m_uMsg == WM_NCCREATE )
	{
		w_hWnd = m_hWnd;

		SetWindowPos( m_hWnd, 0, 100, 100, 1200, 850, SWP_SHOWWINDOW );
	}

	else if ( m_uMsg == WM_PAINT )
	{
		PAINTSTRUCT t_xPS;

		HDC tDC = BeginPaint( m_hWnd, &t_xPS );
		{
			BitBlt( tDC, 0, 0, 1400, 1050, 0, 0, 0, BLACKNESS );

			if ( aa )
			{
				BLENDFUNCTION blenda = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
				float aoe = (float) (GetTickCount() % 628 ) / 100;
				RECT rrr;

	int x, y ;				POINT ppp;
				CF_WS *g = DeriveCF_WS( aa, aoe );
				if ( g )
				{
					rrr.top = rrr.left = 0;
					rrr.right = g _X + 8;
					rrr.bottom = g _Y + 8;
					DrawEdge( tDC, &rrr, EDGE_RAISED, BF_RECT );
					InflateRect( &rrr, -2, -2 );
					DrawEdge( tDC, &rrr, EDGE_SUNKEN, BF_RECT );

				//	GetCursorPos( &ppp );
					AlphaBlend( tDC, 4, 4, g _X, g _Y, g->hDC, 0, 0, g _X, g _Y, blenda );

					KillCF_WS( g );
				}
			}
		}
		EndPaint( m_hWnd, &t_xPS );
	}

	else if ( m_uMsg == WM_RATMESSAGE )
	{
		HDC xDC = GetDC( 0 );
		char buffy[ 1024 ];

		InvalidateRect( w_hWnd, 0, 0 );

		wsprintf( buffy, "w : %08X, l : %08X", m_wParam, m_lParam );
		TextOut( xDC, 100, 100, buffy, lstrlen( buffy ) );
		ReleaseDC( 0, xDC );
	}

	else if ( m_uMsg == WM_CLOSE );

	else if ( m_uMsg == WM_DESTROY )
	{
		PostQuitMessage( 0 );
	}

	return DefWindowProc( m_hWnd, m_uMsg, m_wParam, m_lParam );
}

int WinMainCRTStartup()
{
	aa = LoadCF_WS( 0x40 );

	w_hInst = GetModuleHandle( 0 );
	{
		HWND t_hWnd = CreateClassWindow( w_hInst, "2ND_HAND_MAINGUI_CLASS", "2nd Hand - MainGUI", m_pProc, WS_VISIBLE | WS_OVERLAPPEDWINDOW, 0 );

		if  ( !t_hWnd ) return 0x100;
	}

	RatInit( w_hWnd );

	return MessagePump();
}
