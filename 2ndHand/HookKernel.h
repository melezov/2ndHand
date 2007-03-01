#ifndef _HOOKKERNEL_H_
#define _HOOKKERNEL_H_

	#include "..\\SharedIncludes.h"

	DLLPROC HWND RatAction( HWND x_hWnd ); // @1 NONAME

	BOOL TrayAction( HICON t_hIcon, CTRING t_sTip, BYTE t_bAction );

#endif // _HOOKKERNEL_H_
