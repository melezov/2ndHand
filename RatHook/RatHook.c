#include "..\\2ndHand\\SharedCode.h"

HINSTANCE gr_hInst = 0;
HHOOK gr_hRat = 0;

HWND gx_hWnd = 0;

LRESULT CALLBACK RatProc( int r_iCode, WPARAM r_wParam, LPARAM r_lParam )
{
	if ( gx_hWnd )
	{
		MSLLHOOKSTRUCT *r_xMH = (MSLLHOOKSTRUCT *) r_lParam;

		LPARAM r_lMPosn = MAKELPARAM( r_xMH->pt.x, r_xMH->pt.y );
		WPARAM r_wMData = MAKEWPARAM( r_wParam, HIWORD( r_xMH->mouseData ) );

		PostMessage( gx_hWnd, WM_RATHOOK, r_wMData, r_lMPosn );
	}

	if ( gr_hRat )
	{
		CallNextHookEx( gr_hRat, r_iCode, r_wParam, r_lParam );
	}

	return 0;
}

HWND RatAction( HWND x_hWnd )
{
	if ( x_hWnd )
	{
		if ( !gr_hRat )
		{
			gr_hRat = SetWindowsHookEx( WH_MOUSE_LL, RatProc, gr_hInst, 0 );

			if ( !gr_hRat ) return 0;
		}

		return gx_hWnd = x_hWnd;
	}
	else
	{
		if ( gr_hRat )
		{
			if ( UnhookWindowsHookEx( gr_hRat ) )
			{
				HWND o_hWnd = gx_hWnd;
				gx_hWnd = 0;
				gr_hRat = 0;

				return o_hWnd;
			}
		}

		return 0;
	}
}

int WINAPI _DllMainCRTStartup( HINSTANCE r_hInst, DWORD r_dReason, void *r_vReserved )
{
	if ( r_dReason == DLL_PROCESS_ATTACH )
	{
		if ( !gr_hInst ) gr_hInst = r_hInst;
	}

	else if ( r_dReason == DLL_PROCESS_DETACH )
	{
		RatAction( 0 );
	}

	return 1;
}
