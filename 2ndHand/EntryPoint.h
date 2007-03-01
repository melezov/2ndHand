#ifndef _ENTRYPOINT_H_
#define _ENTRYPOINT_H_

	#include "..\\SharedIncludes.h"

	#define WS_ENTRYPOINT WS_OVERLAPPEDWINDOW | WS_VISIBLE
	#define WX_ENTRYPOINT WS_EX_PALETTEWINDOW

	HWND CreateClassWindow( CTRING s_sCN, CTRING s_sWN, WNDPROC s_pProc, DWORD s_dStyle, DWORD s_dStyleEx );

	void ChangeRatMode( HWND x_hWnd );

	extern WORD WM_TRAYCREA;
	void TrayParse( WPARAM t_wParam, LPARAM t_lParam );

#endif // _ENTRYPOINT_H_
