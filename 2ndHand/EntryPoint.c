#include "EntryPoint.h"

HWND ge_hWnd;
HINSTANCE ge_hInst;

CTRING ge_sCN = TEXT( "2NDHAND_MAINGUI_CLASS" );
CTRING ge_sWN = TEXT( "2ndHand - MainGUI" );

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
		TCHAR koko[ 1024 ];
		HDC tDC = GetDC( 0 );

		wsprintf( koko, TEXT( "%08X - %08X" ), e_wParam, e_lParam );
		TextOut( tDC, 100, 100, koko, lstrlen( koko ) );
		ReleaseDC( 0, tDC );

		return 0;
	}

	else if ( e_uMsg == WM_CLOSE );

	else if ( e_uMsg == WM_DESTROY )
	{
	//	ChangeRatMode( 0 );
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

int WinMainCRTStartup()
{
	ge_hInst = GetModuleHandle( 0 );

	if ( !CreateClassWindow( ge_sCN, ge_sWN, e_pProc, WS_ENTRYPOINT, WX_ENTRYPOINT ) ) return ERROR_ENTRYPOINT( 0 );

	SetWindowPos( ge_hWnd, HWND_TOPMOST, 700, 600, 400, 300, SWP_SHOWWINDOW );

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
