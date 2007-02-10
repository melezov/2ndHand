#include "2ndHand.h"

NOTIFYICONDATA s_NID;

LRESULT CALLBACK ServerWindowProc( HWND s_hWnd, UINT s_uMsg, WPARAM s_wParam, LPARAM s_lParam )
{
    if ( s_uMsg == WM_DESTROY )
	{
		PostQuitMessage( 0 );
	}

    else if ( s_uMsg == WM_USER )
	{
		if ( s_lParam == WM_RBUTTONDOWN )
		{
			HMENU s_hTM = CreatePopupMenu();

			AppendMenu( s_hTM, MF_STRING, WM_DESTROY, "Exit" );

			POINT cPos;
			GetCursorPos( &cPos );

			SetForegroundWindow( s_hWnd );
			int s_iTM = TrackPopupMenuEx( s_hTM, TPM_RETURNCMD | TPM_RIGHTALIGN | TPM_RIGHTBUTTON |/* TPM_NOANIMATION*/0x4000L| TPM_BOTTOMALIGN | TPM_NONOTIFY | TPM_VERTICAL, cPos.x, cPos.y, s_hWnd, 0 );

			DestroyMenu( s_hTM );

			if ( s_iTM == WM_DESTROY ) DestroyWindow( s_hWnd );
		}
	}

	else
	{
		static DWORD WM_TRAYCREA = RegisterWindowMessage( "TaskbarCreated" );

		if  ( s_uMsg == WM_TRAYCREA )
		{
			Shell_NotifyIcon( NIM_ADD, &s_NID );
		}

		else return DefWindowProc( s_hWnd, s_uMsg, s_wParam, s_lParam );
	}

    return 0;
}

int RunServer()
{
	WNDCLASSEX w_MW = { 0 };
	{
		w_MW.cbSize = sizeof( WNDCLASSEX );
		w_MW.hInstance = g_hInst;
		w_MW.lpszClassName = g_sCN;
		w_MW.lpfnWndProc = ServerWindowProc;
	}

	if ( !RegisterClassEx( &w_MW ) ) return 0x400;

	HWND s_hWnd = CreateWindowEx( 0, g_sCN, g_sWN, WS_MINIMIZE, 0, 0, 0, 0, 0, 0, g_hInst, 0 );
	if ( !s_hWnd ) return 0x401;

    {
		s_NID.cbSize	= sizeof( NOTIFYICONDATA );
		s_NID.hWnd		= s_hWnd;
        s_NID.hIcon  	= LoadIcon( g_hInst, (LPCSTR) 0x100 );
		s_NID.uFlags	= NIF_MESSAGE | NIF_ICON | NIF_TIP;

		lstrcpy( s_NID.szTip, g_sWN );
		s_NID.uCallbackMessage = WM_USER;
	}

    Shell_NotifyIcon( NIM_ADD, &s_NID );

	MSG s_Msg;

    while( 1 )
	{
		BOOL t_i = GetMessage( &s_Msg, 0, 0, 0 );

		if ( t_i == -1 ) continue;
		if ( t_i ==  0 ) break;

		TranslateMessage( &s_Msg );
		DispatchMessage( &s_Msg );
	}

    Shell_NotifyIcon( NIM_DELETE, &s_NID );

    return (int) s_Msg.wParam;
}

DWORD WINAPI RunServerHandler( LPVOID s_vParam )
{
	int s_ExitCode = RunServer();

    shutdown( (SOCKET) s_vParam , 2 );
	closesocket( (SOCKET) s_vParam );

	return s_ExitCode;
}
