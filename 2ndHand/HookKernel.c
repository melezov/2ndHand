#include "HookKernel.h"

HWND gh_hActive = 0;

void ChangeRatMode( HWND x_hWnd )
{
	TCHAR buffy[ MAX_PATH ];
	HICON h_hIcon;

	if ( x_hWnd == HWND_MESSAGE )
	{
		if ( !gh_hActive ) x_hWnd = ge_hWnd; else x_hWnd = 0;
	}

	gh_hActive = x_hWnd;
	RatAction( x_hWnd );

	h_hIcon = LoadIcon( ge_hInst, MAKEINTRESOURCE( x_hWnd ? IDI_POINTER : IDI_DENIED ) );
	wsprintf( buffy, TEXT( "%s - %s" ), ge_sWN, x_hWnd ? TEXT( "Active" ) : TEXT( "Disabled" ) );

	TrayAction( h_hIcon, buffy, NIM_MODIFY );
}
