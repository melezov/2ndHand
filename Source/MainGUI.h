#ifndef _MAINGUI_H_
#define _MAINGUI_H_

    #include "SharedCode.h"

    #define WS_MAINGUI ( WS_OVERLAPPEDWINDOW )
	#define WX_MAINGUI ( WS_EX_TOOLWINDOW )

    BOOL TrayParse( UINT m_uMsg, LPARAM m_lParam );
    BOOL ChangeRatMode( HWND x_hWnd );

#endif // _MAINGUI_H_
