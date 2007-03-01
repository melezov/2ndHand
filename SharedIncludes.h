#ifndef _SHAREDINCLUDES_H_
#define _SHAREDINCLUDES_H_

	#define _WIN32_WINNT 0x0600
	#include <windows.h>

	#define DLLPROC __declspec( dllimport )

	#define STRING TCHAR *
	#define CTRING const STRING

	#define ERROR_ENTRYPOINT( e_bId )	( 0x100 + e_bId )
	#define ERROR_CURSORSHOP( e_bId )	( 0x200 + e_bId )
	#define ERROR_TRAYIDOL( e_bId )		( 0x300 + e_bId )

	#define WM_TRAYIDOL ( WM_USER + 0x1 )
	#define WM_RATHOOK  ( WM_USER + 0x2 )

	#define IDI_POINTER	0x10
	#define	IDI_PICKER	0x20
	#define	IDI_DENIED	0x30


	extern CTRING ge_sCN;
	extern CTRING ge_sWN;

	extern HWND ge_hWnd;
	extern HINSTANCE ge_hInst;

#endif // _SHAREDINCLUDES_H_
