#ifndef _MAINGUI_H_
#define _MAINGUI_H_

	#include "..\\SharedIncludes.h"

	#include "..\\RatHook\\RatHook.h"
	#include "..\\CursorFactory\\CursorFactory.h"
    #include "..\\NaviCon\\NaviCon.h"

	HWND CreateClassWindow( HINSTANCE s_hInst, LPCSTR s_sCN, LPCSTR s_sWN, WNDPROC s_pProc, DWORD s_dStyle, DWORD s_dStyleEx );
	int MessagePump();

#endif // _MAINGUI_H_
