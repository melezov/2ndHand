#ifndef _SHAREDCODE_H_
#define _SHAREDCODE_H_

    #define _WIN32_WINNT 0x0600
    #include <windows.h>

    #define STRING TCHAR *
    #define CTRING const STRING

//  ---------------------------------------------------------------------------------------------------------------------------

    #define DLLPROC __declspec( dllimport )

    DLLPROC void MemZero( DWORD s_dSize, void *s_vDest );
    DLLPROC void MemCopy( DWORD s_dSize, void *s_vDest, void *s_vSrc );
    DLLPROC void MessagePump();
    DLLPROC HWND RatHookAction( HWND s_hWnd );
    DLLPROC DWORD NRandom( DWORD *s_dRNG );
    DLLPROC DWORD *MemAlloc( DWORD s_dSize );
    DLLPROC void MemFree( void *s_vDest );

//  ---------------------------------------------------------------------------------------------------------------------------

    HWND CreateClassWindow( CTRING s_sCN, CTRING s_sWN, WNDPROC s_pProc, DWORD s_dStyle, DWORD s_dStyleEx );

//  ---------------------------------------------------------------------------------------------------------------------------

    #define WM_TRAYIDOL ( WM_USER + 0x0 )
    #define WM_RATHOOK  ( WM_USER + 0x1 )

//  ---------------------------------------------------------------------------------------------------------------------------

    #define IDI_POINTER 0x10
    #define IDI_PICKER  0x20
    #define IDI_DENIED  0x30

//  ---------------------------------------------------------------------------------------------------------------------------

    #include "SharedData.h"

//  ---------------------------------------------------------------------------------------------------------------------------

    #define ERROR_MAINGUI( x )    ( 0xE0000100 + (BYTE) ( x ) )
    #define ERROR_CURSORSHOP( x ) ( 0xE0000200 + (BYTE) ( x ) )

//  ---------------------------------------------------------------------------------------------------------------------------

#endif // _SHAREDCODE_H_
