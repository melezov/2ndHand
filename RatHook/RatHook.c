#include "RatHook.h"

HHOOK g_hRat = 0;
HINSTANCE g_hInst = 0;

HWND g_hWnd;

LRESULT CALLBACK RatProc( int r_iCode, WPARAM r_wParam, LPARAM r_lParam )
{
	MSLLHOOKSTRUCT *r_xMH = (MSLLHOOKSTRUCT *) r_lParam;
	{
		LPARAM r_lMPosn = ( (short) r_xMH->pt.x << 16 ) + ( (short) r_xMH->pt.y );
		WPARAM r_wMData = r_xMH->mouseData;

		PostMessage( g_hWnd, WM_RATMESSAGE, r_wMData, r_lMPosn );
	}

	CallNextHookEx( g_hRat, r_iCode, r_wParam, r_lParam );

	return 0;
}

HHOOK RatAction( int r_oAction ) // @2 : NONAME
{
	if ( r_oAction && !g_hRat )
	{
		g_hRat = SetWindowsHookEx( WH_MOUSE_LL, RatProc, g_hInst, 0 );
	}
	else if ( g_hRat )
	{
		UnhookWindowsHookEx( g_hRat );
		g_hRat = 0;
	}

	return g_hRat;
}

HHOOK RatInit( const HWND t_hWnd ) // @1 : NONAME
{
	g_hWnd = t_hWnd;

	return RatAction( 1 );
}

int WINAPI _DllMainCRTStartup( HINSTANCE t_hInst, DWORD r_dReason, void *r_vReserved )
{
	if ( r_dReason == DLL_PROCESS_ATTACH )
	{
		if ( !g_hInst ) g_hInst = t_hInst;
	}

	else if ( r_dReason == DLL_PROCESS_DETACH )
	{
		if ( r_vReserved ) RatAction( 0 );
	}

	return 1;
}
