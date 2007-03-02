#ifndef _CLIENTSPACE_H_
#define _CLIENTSPACE_H_

	#include "..\\SharedIncludes.h"

	#define WS_CURSORPREVIEW ( WS_BORDER | WS_CAPTION | WS_SYSMENU | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_THICKFRAME )
	#define WX_CURSORPREVIEW ( WS_EX_TOOLWINDOW )

	#include "CursorShop.h"

	HWND CreateClassWindow( CTRING s_sCN, CTRING s_sWN, WNDPROC s_pProc, DWORD s_dStyle, DWORD s_dStyleEx );

#endif // _CLIENTSPACE_H_
