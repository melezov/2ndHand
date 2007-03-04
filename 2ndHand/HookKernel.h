#ifndef _HOOKKERNEL_H_
#define _HOOKKERNEL_H_

	#include "..\\SharedIncludes.h"

	#define WS_HOOKKERNEL ( WS_CAPTION | WS_SYSMENU | WS_VISIBLE | WS_THICKFRAME )
	#define WX_HOOKKERNEL ( WS_EX_TOOLWINDOW )

	DLLPROC HWND RatAction( HWND x_hWnd ); // @1 NONAME

	BOOL TrayAction( HICON t_hIcon, CTRING t_sTip, BYTE t_bAction );
	HWND CreateClassWindow( CTRING s_sCN, CTRING s_sWN, WNDPROC s_pProc, DWORD s_dStyle, DWORD s_dStyleEx );

	#include <RichEdit.h>
	#include "CursorShop.h"

#endif // _HOOKKERNEL_H_
