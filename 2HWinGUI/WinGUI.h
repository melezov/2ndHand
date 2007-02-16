#ifndef _2NDHAND_H_
#define _2NDHAND_H_

#define _WIN32_WINNT 0x0500
#define WIN32_LEAN_AND_MEAN

#include "CursorAssembly.h"

#include <shellapi.h>

#pragma comment( lib, "2HRatHook.lib" )

extern "C"
{
	__declspec( dllimport ) void RatInit( const HWND t_hWnd );
	__declspec( dllimport ) void RatAction( bool r_oAction );
}

LRESULT	TrayMenu( HWND w_hWnd, UINT w_uMsg, WPARAM w_wParam, LPARAM w_lParam );
void EvokeRat( HWND w_hWnd, bool t_oActive );

#endif // _2NDHAND_H_
