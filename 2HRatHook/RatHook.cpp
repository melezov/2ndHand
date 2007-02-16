#include "RatHook.h"

HHOOK g_hRat = 0;
HINSTANCE g_hInst = 0;

HWND g_hWnd;

extern "C"
{
	LRESULT CALLBACK RatProc( int r_iCode, WPARAM r_wParam, LPARAM r_lParam )
	{
		CallNextHookEx( g_hRat, r_iCode, r_wParam, r_lParam );

		MSLLHOOKSTRUCT *r_xMH = (MSLLHOOKSTRUCT *) r_lParam;
		PostMessage( g_hWnd, WM_USER + 1, r_xMH->pt.x, r_xMH->pt.y );

		return 0;
	}

	__declspec( dllexport ) void RatAction( bool r_oAction )
	{
		if ( r_oAction && ( g_hRat == 0 ) )
		{
			g_hRat = SetWindowsHookEx( WH_MOUSE_LL, RatProc, g_hInst, 0 );

			ShowWindow( g_hWnd, SW_SHOW );
		}
		else if ( g_hRat )
		{
			UnhookWindowsHookEx( g_hRat );
			g_hRat = 0;

			ShowWindow( g_hWnd, SW_HIDE );
		}
	}

	__declspec( dllexport ) void RatInit( const HWND t_hWnd )
	{
		g_hWnd = t_hWnd;

		RatAction( true );
	}

	int WINAPI _DllMainCRTStartup( HINSTANCE t_hInst, DWORD r_dReason, void *r_vReserved )
	{
		if ( !g_hInst ) g_hInst = t_hInst;

		return 1;
	}
}
