#include "WinGUI.h"

extern const char *w_sWN;
extern HINSTANCE g_hInst;

bool w_oActive = true;

NOTIFYICONDATA w_xNID;

#define WM_TRAYMENU WM_USER
DWORD   WM_TRAYCREA = RegisterWindowMessage( "TaskbarCreated" );

void TrayAction( HWND w_hWnd, DWORD w_dIcon, DWORD w_dAction )
{
	w_xNID.cbSize = sizeof( NOTIFYICONDATA );
	w_xNID.hIcon = LoadIcon( g_hInst, MAKEINTRESOURCE( w_dIcon ) );
	w_xNID.hWnd = w_hWnd;
	w_xNID.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;

	wsprintf( w_xNID.szTip, "%s - %s", w_sWN, w_oActive ? "Active" : "Standby" );
	w_xNID.uCallbackMessage = WM_TRAYMENU;

	Shell_NotifyIcon( w_dAction, &w_xNID );
}

LRESULT	TrayMenu( HWND w_hWnd, UINT w_uMsg, WPARAM w_wParam, LPARAM w_lParam )
{
	if ( ( w_uMsg == WM_CREATE ) || ( w_uMsg == WM_TRAYCREA ) )
	{
		TrayAction( w_hWnd, 0x10, NIM_ADD );
	}

	else if ( w_uMsg == WM_TRAYMENU )
	{
		if ( w_lParam == WM_LBUTTONDBLCLK )
		{
			RatAction( w_oActive = ! w_oActive );
			TrayAction( w_hWnd, w_oActive ? 0x10 : 0x30, NIM_MODIFY );
		}

		else if ( w_lParam == WM_RBUTTONDOWN )
		{
			HMENU w_hTM = CreatePopupMenu();

			AppendMenu( w_hTM, MF_STRING, WM_DESTROY, "Exit 2nd Hand" );

			POINT cPos;
			GetCursorPos( &cPos );

			SetForegroundWindow( w_hWnd );
			TrayAction( w_hWnd, 0x20, NIM_MODIFY );
			int w_iTM = TrackPopupMenuEx( w_hTM, TPM_RETURNCMD | TPM_RIGHTALIGN | TPM_RIGHTBUTTON | TPM_BOTTOMALIGN | TPM_NONOTIFY | TPM_VERTICAL, cPos.x, cPos.y, w_hWnd, 0 );
			TrayAction( w_hWnd, w_oActive ? 0x10 : 0x30, NIM_MODIFY );

			DestroyMenu( w_hTM );

			if ( w_iTM == WM_DESTROY ) DestroyWindow( w_hWnd );
		}
	}

	else if ( w_uMsg == WM_DESTROY )
	{
		TrayAction( w_hWnd, 0, NIM_DELETE );
		return 1;
	}

	else return 1;

	return 0;
}
