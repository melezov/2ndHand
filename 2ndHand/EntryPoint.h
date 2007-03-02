#ifndef _ENTRYPOINT_H_
#define _ENTRYPOINT_H_

	#include "..\\SharedIncludes.h"

#define WS_ENTRYPOINT ( WS_CAPTION | WS_SYSMENU | WS_VISIBLE | WS_THICKFRAME )
#define WX_ENTRYPOINT ( WS_EX_TOOLWINDOW )

	HWND CreateClassWindow( CTRING s_sCN, CTRING s_sWN, WNDPROC s_pProc, DWORD s_dStyle, DWORD s_dStyleEx );

	void ChangeRatMode( HWND x_hWnd );

	extern WORD WM_TRAYCREA;
	void TrayParse( WPARAM t_wParam, LPARAM t_lParam );

	#include "CursorShop.h"

#endif // _ENTRYPOINT_H_
