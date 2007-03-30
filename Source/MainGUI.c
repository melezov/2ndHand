#include "MainGUI.h"

CTRING gm_sCN = TEXT( "2NDHAND_MAINGUI_CLASS" );
CTRING gm_sWN = TEXT( "2ndHand - MainGUI" );

HINSTANCE gm_hInst;
HWND gm_hWnd;

void TestCenter( int m_uMsg );

LRESULT CALLBACK m_pProc( HWND m_hWnd, UINT m_uMsg, WPARAM m_wParam, LPARAM m_lParam )
{
	if ( !TrayParse( m_uMsg, m_lParam ) ) return 0;

    TestCenter( m_uMsg );

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

int MainGUIEntryPoint()
{
    gm_hInst = GetModuleHandle( 0 );
    gm_hWnd = CreateClassWindow( gm_sCN, gm_sWN, m_pProc, WS_MAINGUI, WX_MAINGUI );

    if ( gm_hWnd )
    {
        SetWindowPos( gm_hWnd, 0, 50, 50, 1300, 850, SWP_SHOWWINDOW );
        MessagePump();
    }

    return ERROR_MAINGUI( 0 );
}
