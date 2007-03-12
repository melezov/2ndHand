#ifndef _SHAREDINCLUDES_H_
#define _SHAREDINCLUDES_H_

	#define _WIN32_WINNT 0x0600
	#include <windows.h>

//	----------------------------------------------------------------------------------------------------------------------------

	#define DLLPROC __declspec( dllimport )

	#define STRING TCHAR *
	#define CTRING const STRING

	#define ERROR_MAINGUI( x )			( 0x100 + x )
	#define ERROR_TRAYIDOL( x )			( 0x200 + x )
	#define ERROR_CURSORSHOP( x )		( 0x300 + x )
	#define ERROR_CURSORPREVIEW( x )	( 0x400 + x )

//	----------------------------------------------------------------------------------------------------------------------------

	#define WM_TRAYIDOL ( WM_USER + 0x0 )
	#define WM_RATHOOK  ( WM_USER + 0x1 )

//	----------------------------------------------------------------------------------------------------------------------------

	#define IDI_POINTER	0x10
	#define	IDI_PICKER	0x20
	#define	IDI_DENIED	0x30

//	----------------------------------------------------------------------------------------------------------------------------

	extern HINSTANCE gm_hInst;
	extern CTRING gm_sCN;
	extern CTRING gm_sWN;
	extern HWND gm_hWnd;

	extern CTRING gp_sCN;
	extern CTRING gp_sWN;

	extern CTRING gc_sCN;
	extern CTRING gc_sWN;

//	----------------------------------------------------------------------------------------------------------------------------

	HWND CreateClassWindow( CTRING s_sCN, CTRING s_sWN, WNDPROC s_pProc, DWORD s_dStyle, DWORD s_dStyleEx );

#endif // _SHAREDINCLUDES_H_
