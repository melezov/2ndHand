#ifndef _RATHOOK_H_
#define _RATHOOK_H_

	#ifdef _SHAREDINCLUDES_H_

		__declspec( dllimport ) HHOOK RatInit( const HWND t_hWnd );	// @1 NONAME
		__declspec( dllimport ) HHOOK RatAction( int r_oAction );	// @2 NONAME

	#endif // _SHAREDINCLUDES_H_

	#include "..\\SharedIncludes.h"

	#define WM_RATMESSAGE ( WM_USER + 0x100 )

#endif // _RATHOOK_H_
