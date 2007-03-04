#include "EntryPoint.h"

extern HINSTANCE ge_hInst;

HWND CreateClassWindow( CTRING s_sCN, CTRING s_sWN, WNDPROC s_pProc, DWORD s_dStyle, DWORD s_dStyleEx )
{
	WNDCLASSEX s_xWCX = { 0 };
	{
		s_xWCX.cbSize = sizeof( WNDCLASSEX );
		s_xWCX.hInstance = ge_hInst;
		s_xWCX.lpszClassName = s_sCN;
		s_xWCX.lpfnWndProc = s_pProc;
		s_xWCX.hCursor = LoadCursor( 0, IDC_ARROW );

		if ( s_sWN == ge_sWN ) s_xWCX.hIcon = LoadIcon( ge_hInst, MAKEINTRESOURCE( IDI_POINTER ) );

		if ( !RegisterClassEx( &s_xWCX ) )
		{
			if ( GetLastError() != ERROR_CLASS_ALREADY_EXISTS )	return 0;
		}
	}

	return CreateWindowEx( s_dStyleEx, s_sCN, s_sWN, s_dStyle, 0, 0, 0, 0, 0, 0, 0, 0 );
}

extern HWND ge_hBox, gh_hBox;

void OutputText( WPARAM wId, CTRING sText )
{
	TCHAR buffy[ MAX_PATH * 100 ];
	CTRING sId;

	HWND gx_hBox = ge_hBox;
	CHARFORMAT x_CF = { 0 };

	SYSTEMTIME x_ST;
	GetSystemTime( &x_ST );

	x_CF.cbSize = sizeof( CHARFORMAT );
	x_CF.dwMask = CFM_COLOR;

	switch ( wId )
	{
		case OT_MAINGUI:		sId = TEXT( "MainGUI" );	x_CF.crTextColor = 0xffffff; break;
		case OT_TRAYIDOL:		sId = TEXT( "TrayIdol" );	x_CF.crTextColor = 0xafffaf; break;
		case OT_HOOKKERNEL:		sId = TEXT( "HookKernel" );	x_CF.crTextColor = 0xafafff; break;
		case OT_RATHOOK:		sId = TEXT( "RatHook" );	x_CF.crTextColor = 0xffafaf; break;

		default:

		gx_hBox = gh_hBox;

		switch ( LOWORD( wId ) )
		{
			case WM_MOUSEMOVE:		sId = TEXT( "Mouse move" );		x_CF.crTextColor = 0x7f7f7f; break;
			case WM_LBUTTONDOWN:	sId = TEXT( "L button down" );	x_CF.crTextColor = 0xffff7f; break;
			case WM_LBUTTONUP:		sId = TEXT( "L button up" );	x_CF.crTextColor = 0xff7fff; break;
			case WM_MBUTTONDOWN:	sId = TEXT( "M button down" );	x_CF.crTextColor = 0x7fffff; break;
			case WM_MBUTTONUP:		sId = TEXT( "M button up" );	x_CF.crTextColor = 0xff7f7f; break;
			case WM_RBUTTONDOWN:	sId = TEXT( "R button down" );	x_CF.crTextColor = 0x7f7fff; break;
			case WM_RBUTTONUP:		sId = TEXT( "R button up" );	x_CF.crTextColor = 0x7fff7f; break;
			case WM_MOUSEWHEEL:		sId = TEXT( "Mouse wheel" );	x_CF.crTextColor = 0xffffff; break;

			default:
			{
				sId = TEXT( "Unmapped" ); x_CF.crTextColor = 0x7f7f7f;
			}
		}
	}

	SendMessage( gx_hBox, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM) &x_CF );

	SendMessage( gx_hBox, EM_SETSEL, 1 << 20, 1 << 20 );
	wsprintf( buffy, TEXT( "\r\n%02d:%02d:%02d - %s: %s" ), x_ST.wHour, x_ST.wMinute, x_ST.wSecond, sId, sText );
	SendMessage( gx_hBox, EM_REPLACESEL, 0, (LPARAM) buffy );
}
