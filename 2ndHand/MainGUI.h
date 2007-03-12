#ifndef _ENTRYPOINT_H_
#define _ENTRYPOINT_H_

	#include "SharedCode.h"

	#define WS_MAINGUI ( WS_ICONIC | WS_SYSMENU )// | WS_CAPTION | WS_VISIBLE | WS_OVERLAPPEDWINDOW ) // WS_ICONIC )
	#define WX_MAINGUI ( WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE ) // ( WS_EX_TOOLWINDOW ) // | WS_EX_NOACTIVATE )

	extern WORD WM_TRAYCREA;
	void TrayParse( WPARAM t_wParam, LPARAM t_lParam );
	BOOL TrayAction( HICON t_hIcon, CTRING t_sTip, BYTE t_bAction );

	DLLPROC HWND RatAction( HWND x_hWnd ); // @1 NONAME

	void MoveLaddy( WPARAM m_wParam, LPARAM m_lParam );

#endif // _ENTRYPOINT_H_
