#ifndef _ENTRYPOINT_H_
#define _ENTRYPOINT_H_

	#include "SharedCode.h"

	#define WS_MAINGUI ( WS_OVERLAPPEDWINDOW ) // ( WS_ICONIC | WS_SYSMENU )
	#define WX_MAINGUI ( 0 ) // ( WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE )

	BOOL TrayAction( HICON m_hIcon, CTRING m_sTip, BYTE m_bAction );
	BOOL TrayParse( UINT m_uMsg, WPARAM m_wParam, LPARAM m_lParam );

	DLLPROC HWND RatAction( HWND x_hWnd ); // @1 NONAME
	BOOL ChangeRatMode( HWND x_hWnd );

//	void MoveLaddy( WPARAM m_wParam, LPARAM m_lParam );

#endif // _ENTRYPOINT_H_
