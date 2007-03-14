#include "MainGUI.h"

UINT WM_TRAYCREA = WM_TRAYIDOL;
HWND gh_hActive;

BOOL TrayAction( HICON m_hIcon, CTRING m_sTip, BYTE m_bAction )
{
	static NOTIFYICONDATA m_xNID = { 0 };

	if ( !m_xNID.cbSize )
	{
		m_xNID.cbSize = sizeof( NOTIFYICONDATA );
		m_xNID.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;

		m_xNID.hWnd = gm_hWnd;
		m_xNID.uCallbackMessage = WM_TRAYIDOL;

		WM_TRAYCREA = RegisterWindowMessage( TEXT( "TaskbarCreated" ) );
	}

	if ( m_hIcon ) m_xNID.hIcon = m_hIcon;
	if ( m_sTip  ) wsprintf( m_xNID.szTip, m_sTip );

	if ( !Shell_NotifyIcon( m_bAction, &m_xNID ) ) return ERROR_MAINGUI( -1 );

	return ERROR_SUCCESS;
}

BOOL ChangeRatMode( HWND x_hWnd )
{
	TCHAR buffy[ MAX_PATH ];
	HICON h_hIcon;

	gh_hActive = x_hWnd;
	RatAction( x_hWnd );

	h_hIcon = LoadIcon( gm_hInst, MAKEINTRESOURCE( x_hWnd ? IDI_POINTER : IDI_DENIED ) );
	wsprintf( buffy, TEXT( "%s - %s" ), gm_sWN, x_hWnd ? TEXT( "Active" ) : TEXT( "Disabled" ) );

	return TrayAction( h_hIcon, buffy, NIM_MODIFY );
}

BOOL TrayParse( UINT m_uMsg, WPARAM m_wParam, LPARAM m_lParam )
{
	if ( ( m_uMsg == WM_CREATE ) || ( m_uMsg == WM_TRAYCREA ) || ( m_uMsg == WM_NCDESTROY ) )
	{
		TrayAction( 0, 0, m_uMsg != WM_NCDESTROY ? NIM_ADD : NIM_DELETE );
	}

	else if ( m_uMsg == WM_TRAYIDOL )
	{
		if ( m_lParam == WM_LBUTTONDBLCLK )
		{
			ChangeRatMode( !gh_hActive ? gm_hWnd : 0 );
		}

		else if ( m_lParam == WM_RBUTTONDOWN )
		{
			int m_iTM;
			POINT m_cPos;
			HMENU m_hTM = CreatePopupMenu();

	//		AppendMenu( m_hTM, MF_STRING, WM_CREATE, TEXT( "Open Cursor ..." ) );
	//		AppendMenu( m_hTM, MF_SEPARATOR, 0, 0 );
			AppendMenu( m_hTM, MF_STRING, WM_DESTROY, TEXT( "Exit 2nd Hand" ) );

			GetCursorPos( &m_cPos );
			SetForegroundWindow( gm_hWnd );
			m_iTM = TrackPopupMenuEx( m_hTM, TPM_RETURNCMD | TPM_RIGHTALIGN | TPM_RIGHTBUTTON | TPM_BOTTOMALIGN | TPM_NONOTIFY | TPM_VERTICAL, m_cPos.x, m_cPos.y, gm_hWnd, 0 );
			DestroyMenu( m_hTM );

	/*		if ( m_iTM == WM_CREATE )
			{
				CreateClient();
			}

			else
	*/
			if ( m_iTM == WM_DESTROY )
			{
				PostMessage( gm_hWnd, WM_CLOSE, 0, 0 );
			}
		}

		return 0;
	}

	return 1;
}
