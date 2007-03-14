#ifndef _SHAREDINCLUDES_H_
#define _SHAREDINCLUDES_H_

	#define _WIN32_WINNT 0x0600
	#include <windows.h>

	#define DLLPROC __declspec( dllimport )

//	----------------------------------------------------------------------------------------------------------------------------

	#define STRING TCHAR *
	#define CTRING const STRING

	BOOL ERROR_MESSAGE( CTRING eText, BOOL eID );

	#define	ERROR_MAINGUI( iD )		ERROR_MESSAGE( "MainGUI", 0x100 + iD )
	#define	ERROR_CURSORSHOP( iD )	ERROR_MESSAGE( "CursorShop", 0x200 + iD )

//	#define ERROR_CURSORPREVIEW( x )	( 0x300 + x )

//	----------------------------------------------------------------------------------------------------------------------------

	#define WM_TRAYIDOL ( WM_USER + 0x0 )
	#define WM_RATHOOK  ( WM_USER + 0x1 )

//	----------------------------------------------------------------------------------------------------------------------------

	#define IDI_POINTER	0x10
	#define	IDI_PICKER	0x20
	#define	IDI_DENIED	0x30

//	----------------------------------------------------------------------------------------------------------------------------

	extern HINSTANCE gm_hInst;
	extern HWND gm_hWnd;

	extern CTRING gm_sCN;
	extern CTRING gm_sWN;

//	extern CTRING gp_sCN;
//	extern CTRING gp_sWN;

//	extern CTRING gc_sCN;
//	extern CTRING gc_sWN;

//	----------------------------------------------------------------------------------------------------------------------------

	HWND CreateClassWindow( CTRING s_sCN, CTRING s_sWN, WNDPROC s_pProc, DWORD s_dStyle, DWORD s_dStyleEx );

#endif // _SHAREDINCLUDES_H_
