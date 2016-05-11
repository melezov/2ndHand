#include "MainGxUI.h"
#include "TrayIdol.h"

LRESULT TestParse( UINT m_uMsg, WPARAM m_wParam, LPARAM m_lParam );

LRESULT CALLBACK m_pProc( HWND m_hWnd, UINT m_uMsg, WPARAM m_wParam, LPARAM m_lParam )
{
    if ( m_uMsg == WM_NCCREATE )
    {
        gm_hWnd = m_hWnd;
    }

    if ( !TrayParse( m_uMsg, m_lParam ) ) return 0;
    if ( !TestParse( m_uMsg, m_wParam, m_lParam ) ) return 0;

    if ( m_uMsg == WM_DESTROY )
    {
        PostQuitMessage( ERROR_SUCCESS );
    }

    return DefWindowProc( m_hWnd, m_uMsg, m_wParam, m_lParam );
}

DWORD MainGxUI()
{
    if ( !CreateClassWindow( &gm_sCN, &gm_sWN, m_pProc, WS_MAINGXUI, WX_MAINGXUI ) )
    {
        return ERROR_MAINGXUI( 0x00 );
    }

    SetWindowPos( gm_hWnd, 0, 200, 200, 500, 400, SWP_SHOWWINDOW );

    return MessagePump();
}
