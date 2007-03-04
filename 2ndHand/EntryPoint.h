#ifndef _ENTRYPOINT_H_
#define _ENTRYPOINT_H_

	#include "..\\SharedIncludes.h"

	#define WS_ENTRYPOINT ( WS_OVERLAPPEDWINDOW | WS_VISIBLE )
	#define WX_ENTRYPOINT ( 0 )

	HWND CreateClassWindow( CTRING s_sCN, CTRING s_sWN, WNDPROC s_pProc, DWORD s_dStyle, DWORD s_dStyleEx );

	BOOL HookInit();

	extern WORD WM_TRAYCREA;
	void TrayParse( WPARAM t_wParam, LPARAM t_lParam );

	#include <Richedit.h>

#endif // _ENTRYPOINT_H_
