#include "TrayIdol.h"

WORD WM_TRAYCREA = WM_TRAYIDOL;

BOOL TrayAction( HICON t_hIcon, CTRING t_sTip, BYTE t_bAction )
{
	static NOTIFYICONDATA t_xNID = { 0 };

	if ( !t_xNID.cbSize )
	{
		t_xNID.cbSize = sizeof( NOTIFYICONDATA );
		t_xNID.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;

		t_xNID.hWnd = ge_hWnd;
		t_xNID.uCallbackMessage = WM_TRAYIDOL;

		WM_TRAYCREA = RegisterWindowMessage( TEXT( "TaskbarCreated" ) );
	}

	if ( t_hIcon ) t_xNID.hIcon = t_hIcon;
	if ( t_sTip  ) wsprintf( t_xNID.szTip, t_sTip );

	if ( !Shell_NotifyIcon( t_bAction, &t_xNID ) ) return ERROR_TRAYIDOL( t_bAction );

	return 0;
}

void TrayParse( WPARAM t_wParam, LPARAM t_lParam )
{
	if ( t_wParam == WM_TRAYIDOL )
	{
		TrayAction( 0, 0, (BYTE) t_lParam );
	}

	else if ( t_lParam == WM_LBUTTONDBLCLK )
	{
		ChangeRatMode( HWND_MESSAGE );
	}

	else if ( t_lParam == WM_RBUTTONDOWN )
	{
		int t_iTM;
		POINT t_cPos;

		HMENU t_hTM = CreatePopupMenu();

		AppendMenu( t_hTM, gc_hWnd ? MF_CHECKED : MF_STRING, WM_CREATE, TEXT( "Cursor Preview" ) );

		AppendMenu( t_hTM, MF_SEPARATOR, 0, 0 );
		AppendMenu( t_hTM, MF_STRING, WM_DESTROY, TEXT( "Exit 2nd Hand" ) );

		GetCursorPos( &t_cPos );
		t_iTM = TrackPopupMenuEx( t_hTM, TPM_RETURNCMD | TPM_RIGHTALIGN | TPM_RIGHTBUTTON | TPM_BOTTOMALIGN | TPM_NONOTIFY | TPM_VERTICAL, t_cPos.x, t_cPos.y, ge_hWnd, 0 );

		DestroyMenu( t_hTM );

		if ( t_iTM == WM_CREATE )
		{
			CreateClient();
		}

		else if ( t_iTM == WM_DESTROY )
		{
			PostMessage( ge_hWnd, WM_CLOSE, 0, 0 );
		}
	}
}
