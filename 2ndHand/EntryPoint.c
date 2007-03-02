#include "EntryPoint.h"

HWND ge_hWnd;
HINSTANCE ge_hInst;

CTRING ge_sCN = TEXT( "2NDHAND_MAINGUI_CLASS" );
CTRING ge_sWN = TEXT( "2ndHand - MainGUI" );

extern CS_DUMP *g_cPreview;

TCHAR ge_sMI[ MAX_PATH ];

LRESULT CALLBACK e_pProc( HWND e_hWnd, UINT e_uMsg, WPARAM e_wParam, LPARAM e_lParam )
{
	if ( e_uMsg == WM_NCCREATE )
	{
		ge_hWnd = e_hWnd;
		TrayParse( WM_TRAYIDOL, NIM_ADD );
		ChangeRatMode( e_hWnd );
	}

	else if ( e_uMsg == WM_RATHOOK )
	{
		wsprintf( ge_sMI, TEXT( "%08X - %08X" ), e_wParam, e_lParam );
		InvalidateRect( e_hWnd, 0, 0 );

		if ( g_cPreview ) MoveCS_Dump( g_cPreview, e_lParam );

		return 0;
	}

	else if ( e_uMsg == WM_PAINT )
	{
		RECT e_rWnd;
		PAINTSTRUCT x_PS;
		HDC eDC = BeginPaint( e_hWnd, &x_PS );

		GetClientRect( e_hWnd, &e_rWnd );
		BitBlt( eDC, 0, 0, e_rWnd.right, e_rWnd.bottom, 0, 0, 0, BLACKNESS );

		SetTextColor( eDC, 0xffff00 );
		SetBkColor( eDC, 0 );

		DrawText( eDC, ge_sMI, lstrlen( ge_sMI ), &e_rWnd, DT_SINGLELINE | DT_CENTER | DT_VCENTER );
		EndPaint( e_hWnd, &x_PS );
	}

	else if ( e_uMsg == WM_CLOSE );

	else if ( e_uMsg == WM_DESTROY )
	{
		ChangeRatMode( 0 );
		TrayParse( WM_TRAYIDOL, NIM_DELETE );

		PostQuitMessage( 0 );
	}

	else if ( e_uMsg == WM_TRAYCREA )
	{
		TrayParse( WM_TRAYIDOL, NIM_ADD );
		return 0;
	}

	else if ( e_uMsg == WM_TRAYIDOL )
	{
		TrayParse( e_wParam, e_lParam );
		return 0;
	}

	return DefWindowProc( e_hWnd, e_uMsg, e_wParam, e_lParam );
}

void CreateClient();

int WinMainCRTStartup()
{
	ge_hInst = GetModuleHandle( 0 );

	if ( !CreateClassWindow( ge_sCN, ge_sWN, e_pProc, WS_ENTRYPOINT, WX_ENTRYPOINT ) ) return ERROR_ENTRYPOINT( 0 );
	SetWindowPos( ge_hWnd, HWND_TOP, 100, 100, 200, 100, SWP_SHOWWINDOW );

	CreateClient();

	while( 1 )
	{
		MSG e_xMsg;
	 	int e_iMsg = GetMessage( &e_xMsg, 0, 0, 0 );

		if ( !( ++ e_iMsg ) ) continue;
		if ( !( -- e_iMsg ) ) return (int) e_xMsg.wParam;

		TranslateMessage( &e_xMsg );
		DispatchMessage( &e_xMsg );
	}
}
