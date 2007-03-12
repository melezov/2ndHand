#include "MainGUI.h"

void ChangeRatMode( HWND x_hWnd )
{
	static HWND gh_hActive;

	TCHAR buffy[ MAX_PATH ];
	HICON h_hIcon;

	if ( x_hWnd == HWND_MESSAGE )
	{
		if ( !gh_hActive ) x_hWnd = gm_hWnd; else x_hWnd = 0;
	}

	gh_hActive = x_hWnd;
	RatAction( x_hWnd );

	h_hIcon = LoadIcon( gm_hInst, MAKEINTRESOURCE( x_hWnd ? IDI_POINTER : IDI_DENIED ) );
	wsprintf( buffy, TEXT( "%s - %s" ), gm_sWN, x_hWnd ? TEXT( "Active" ) : TEXT( "Disabled" ) );

	TrayAction( h_hIcon, buffy, NIM_MODIFY );
}

LRESULT CALLBACK m_pProc( HWND m_hWnd, UINT m_uMsg, WPARAM m_wParam, LPARAM m_lParam )
{
	if ( m_uMsg == WM_NCCREATE )
	{
		gm_hWnd = m_hWnd;
		TrayParse( WM_TRAYIDOL, NIM_ADD );
		ChangeRatMode( m_hWnd );
	}

	else if ( m_uMsg == WM_DESTROY )
	{
		ChangeRatMode( 0 );
		TrayParse( WM_TRAYIDOL, NIM_DELETE );
		PostQuitMessage( 0 );
	}

	else if ( m_uMsg == WM_TRAYCREA )
	{
		TrayParse( WM_TRAYIDOL, NIM_ADD );
		return 0;
	}

	else if ( m_uMsg == WM_TRAYIDOL )
	{
		TrayParse( m_wParam, m_lParam );
		return 0;
	}

	else if ( m_uMsg == WM_RATHOOK )
	{
		MoveLaddy( m_wParam, m_lParam );
		return 0;
	}

	return DefWindowProc( m_hWnd, m_uMsg, m_wParam, m_lParam );
}

int WinMainCRTStartup()
{
	gm_hInst = GetModuleHandle( 0 );

	if ( !CreateClassWindow( gm_sCN, gm_sWN, m_pProc, WS_MAINGUI, WX_MAINGUI ) ) return ERROR_MAINGUI( 0 );

//	CreateClient();

	while( 1 )
	{
		MSG m_xMsg;
	 	int m_iMsg = GetMessage( &m_xMsg, 0, 0, 0 );

		if ( !( ++ m_iMsg ) ) continue;
		if ( !( -- m_iMsg ) ) ExitProcess( (UINT) m_xMsg.wParam );

		TranslateMessage( &m_xMsg );
		DispatchMessage( &m_xMsg );
	}
}
