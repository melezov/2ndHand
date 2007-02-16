#include "WinGUI.h"

const char *w_sCN = "2ND_HAND_WINGUI_CLASS";
const char *w_sWN = "2nd Hand WinGUI";

HINSTANCE g_hInst;

HWND CreateClassWindow( HINSTANCE t_hInst, LPCSTR t_sCN, LPCSTR t_sWN, WNDPROC t_pProc, DWORD t_dStyle, DWORD t_dStyleEx )
{
	WNDCLASSEX t_xWCX = { 0 };
	{
		t_xWCX.cbSize = sizeof( WNDCLASSEX );
		t_xWCX.hInstance = t_hInst;
		t_xWCX.lpszClassName = t_sCN;
		t_xWCX.lpfnWndProc = t_pProc;

		if ( !RegisterClassEx( &t_xWCX ) ) return 0;
	}

	return CreateWindowEx( t_dStyleEx, t_sCN, t_sWN, t_dStyle, 0, 0, 0, 0, 0, 0, 0, 0 );
}

int MessagePump()
{
    MSG g_xMsg;

	while( 1 )
	{
		BOOL t_i = GetMessage( &g_xMsg, 0, 0, 0 );

		if ( t_i == -1 ) continue;
		if ( t_i ==  0 ) break;

		TranslateMessage( &g_xMsg );
		DispatchMessage( &g_xMsg );
	}

    return (int) g_xMsg.wParam;
}

CA_INFO *curCursor;

LRESULT CALLBACK MainProc( HWND w_hWnd, UINT w_uMsg, WPARAM w_wParam, LPARAM w_lParam )
{
	if ( w_uMsg == WM_USER + 1 )
	{
		POINT w_pNew = { (long) w_wParam, (long) w_lParam };
		AssembleRotatedCursor( w_hWnd, curCursor, w_pNew );
		return 0;
	}

	if ( !TrayMenu( w_hWnd, w_uMsg, w_wParam, w_lParam ) ) return 0;

	if ( w_uMsg == WM_CREATE )
	{
		curCursor = CreateCursorFactory( 0x40 );
		return 0;
	}

	if ( w_uMsg == WM_CLOSE ) return 0;

	if ( w_uMsg == WM_DESTROY )
	{
		RatAction( false );
		PostQuitMessage( 0 );
	}

	return DefWindowProc( w_hWnd, w_uMsg, w_wParam, w_lParam );
}

extern "C" int WinMainCRTStartup()
{
	g_hInst = GetModuleHandle( 0 );

	HWND w_hWnd = CreateClassWindow( g_hInst, w_sCN, w_sWN, MainProc, WS_VISIBLE | WS_POPUP, WS_EX_PALETTEWINDOW | WS_EX_LAYERED | WS_EX_TRANSPARENT );
	if ( !w_hWnd ) return 1;

	RatInit( w_hWnd );

	return MessagePump();
}
