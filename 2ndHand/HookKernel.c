#include "HookKernel.h"

extern HMODULE ge_hDll;

HWND gh_hActive = 0;
HWND gh_hWnd;

CTRING gh_sCN = TEXT( "2NDHAND_HOOKKERNEL_CLASS" );
CTRING gh_sWN = TEXT( "2ndHand - RatHook Info" );

POINTS gh_pOld;

void ChangeRatMode( HWND x_hWnd )
{
	TCHAR buffy[ MAX_PATH ];
	HICON h_hIcon;

	if ( x_hWnd == HWND_MESSAGE )
	{
		if ( !gh_hActive ) x_hWnd = gh_hWnd; else x_hWnd = 0;
	}

	gh_hActive = x_hWnd;
	RatAction( x_hWnd );

	OutputText( OT_RATHOOK, x_hWnd ? TEXT( "Active!" ) : TEXT( "Disabled!" ) );

	h_hIcon = LoadIcon( ge_hInst, MAKEINTRESOURCE( x_hWnd ? IDI_POINTER : IDI_DENIED ) );
	wsprintf( buffy, TEXT( "%s - %s" ), ge_sWN, x_hWnd ? TEXT( "Active" ) : TEXT( "Disabled" ) );

	TrayAction( h_hIcon, buffy, NIM_MODIFY );
	InvalidateRect( gh_hWnd, 0, 0 );
}

HWND gh_hBox;

CS_DUMP *g_cPreview;

LRESULT CALLBACK h_pProc( HWND h_hWnd, UINT h_uMsg, WPARAM h_wParam, LPARAM h_lParam )
{
	if ( h_uMsg == WM_NCCREATE )
	{
		OutputText( OT_HOOKKERNEL, TEXT( "Created!" ) );
		ChangeRatMode( gh_hWnd = h_hWnd );

		gh_hBox = CreateWindowEx( 0, TEXT( "RichEdit20A" ), 0, WS_VSCROLL | WS_BORDER | WS_VISIBLE | WS_CHILD | ES_CENTER | ES_AUTOVSCROLL | ES_MULTILINE | ES_NOHIDESEL | ES_READONLY, 0, 0, 0, 0, h_hWnd, (HMENU) 0x10, 0, 0 );
		SendMessage( gh_hBox, EM_SETBKGNDCOLOR, 0, 0x000000 );
		SendMessage( gh_hBox, EM_EXLIMITTEXT, 1 << 20, 0 );
	}

	else if ( h_uMsg == WM_RATHOOK )
	{
		TCHAR sMI[ MAX_PATH ];
		POINTS pNew;

		pNew.x = LOWORD( h_lParam );
		pNew.y = HIWORD( h_lParam );

		wsprintf( sMI, TEXT( "%08X - %08X" ), h_wParam, h_lParam );
		OutputText( h_wParam, sMI );

		if ( g_cPreview ) MoveCS_Dump( g_cPreview, h_lParam );

		gh_pOld.x = pNew.x;
		gh_pOld.y = pNew.y;

		return 0;
	}

	else if ( h_uMsg == WM_SIZE )
	{
		RECT hBounds;

		GetClientRect( h_hWnd, &hBounds );
		SetWindowPos( gh_hBox, HWND_TOP, 0, 0, hBounds.right, hBounds.bottom, SWP_SHOWWINDOW );
	}

	else if ( h_uMsg == WM_SHOWWINDOW )
	{
		OutputText( OT_HOOKKERNEL, h_wParam ? TEXT( "Showing ..." ) : TEXT( "Hiding ..." ) );
	}

	else if ( h_uMsg == WM_CLOSE )
	{
		ShowWindow( h_hWnd, SW_HIDE );
		return 0;
	}

	else if ( h_uMsg == WM_DESTROY )
	{
		OutputText( OT_HOOKKERNEL, "Destroyed!" );
		ChangeRatMode( 0 );
	}

	return DefWindowProc( h_hWnd, h_uMsg, h_wParam, h_lParam );
}

BOOL HookInit()
{
	if ( !CreateClassWindow( gh_sCN, gh_sWN, h_pProc, WS_HOOKKERNEL, WX_HOOKKERNEL ) ) return ERROR_HOOKKERNEL( 0 );

	SetWindowPos( gh_hWnd, HWND_TOP, 350, 100, 400, 800, SWP_SHOWWINDOW );

	return 0;
}
