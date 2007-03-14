#include "MainGUI.h"
#include "CursorShop.h"

void TestCenter( UINT msg, WPARAM wparam, LPARAM lparam );

LRESULT CALLBACK m_pProc( HWND m_hWnd, UINT m_uMsg, WPARAM m_wParam, LPARAM m_lParam )
{
	if ( !TrayParse( m_uMsg, m_wParam, m_lParam ) ) return 0;

	TestCenter( m_uMsg, m_wParam, m_lParam );

	if ( m_uMsg == WM_NCCREATE )
	{
		gm_hWnd = m_hWnd;
		ChangeRatMode( m_hWnd );
	}

	else if ( m_uMsg == WM_DESTROY )
	{
		ChangeRatMode( 0 );
		PostQuitMessage( 0 );
	}

	return DefWindowProc( m_hWnd, m_uMsg, m_wParam, m_lParam );
}

int WinMainCRTStartup()
{
	gm_hInst = GetModuleHandle( 0 );

	if ( !CreateClassWindow( gm_sCN, gm_sWN, m_pProc, WS_MAINGUI, WX_MAINGUI ) ) return ERROR_MAINGUI( 0 );

	SetWindowPos( gm_hWnd, HWND_TOP, 100, 100, 1200, 850, SWP_SHOWWINDOW );

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
