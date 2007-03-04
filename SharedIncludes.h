#ifndef _SHAREDINCLUDES_H_
#define _SHAREDINCLUDES_H_

	#define _WIN32_WINNT 0x0600
	#include <windows.h>

	#define DLLPROC __declspec( dllimport )

	#define STRING TCHAR *
	#define CTRING const STRING

	#define ERROR_ENTRYPOINT( e_bId )	( 0x100 + e_bId )
	#define ERROR_CURSORSHOP( e_bId )	( 0x200 + e_bId )
	#define ERROR_HOOKKERNEL( e_bId )	( 0x300 + e_bId )
	#define ERROR_TRAYIDOL( e_bId )		( 0x400 + e_bId )

	#define WM_TRAYIDOL ( WM_USER + 0x1 )
	#define WM_RATHOOK  ( WM_USER + 0x2 )

	#define IDI_POINTER	0x10
	#define	IDI_PICKER	0x20
	#define	IDI_DENIED	0x30

	#define OT_MAINGUI		0x0
	#define OT_HOOKKERNEL	0x1
	#define OT_TRAYIDOL		0x2
	#define OT_RATHOOK		0x3

	void OutputText( WPARAM wId, CTRING sText );

	extern CTRING ge_sCN;
	extern CTRING ge_sWN;
	extern HWND ge_hWnd;

	extern CTRING gh_sCN;
	extern CTRING gh_sWN;
	extern HWND gh_hWnd;

	extern CTRING gs_sCN;
	extern CTRING gs_sWN;
	extern HWND gc_hWnd;

	extern HINSTANCE ge_hInst;

#endif // _SHAREDINCLUDES_H_
