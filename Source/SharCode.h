#ifndef _SHARCODE_H_
#define _SHARCODE_H_

//  ---------------------------------------------------------------------------------------------------------------------------

    #include "SharXtra.h"
    #define _WIN32_WINNT 0x0500
    #include <windows.h>

//  ---------------------------------------------------------------------------------------------------------------------------

    #define COAL const TCHAR

    extern LPMALLOC gs_vMem;
    extern HINSTANCE gs_hInst;
    extern COAL gs_sSH;

    extern HWND gm_hWnd;

//  ---------------------------------------------------------------------------------------------------------------------------

    #define CHECK_ERROR( value ) if ( !( value ) ) goto CatchError; dErr ++

//  ---------------------------------------------------------------------------------------------------------------------------

    HWND CreateClassWindow( COAL *s_sCN, COAL *s_sWN, WNDPROC s_pProc, DWORD s_dStyle, DWORD s_dStyleEx );
    DWORD MsgErr( DWORD s_dErr, DWORD s_dIcon );

//  ---------------------------------------------------------------------------------------------------------------------------

    void MemZero( DWORD s_dSize, void *s_vDest );
    void MemCopy( DWORD s_dSize, void *s_vDest, void *s_vSrc );

    DWORD *MemAlloc( DWORD s_dSize );
    void MemFree( void *s_vDest );

    DWORD NRandom( DWORD *s_dRNG, DWORD s_dBase );

//  ---------------------------------------------------------------------------------------------------------------------------

#endif // _SHARCODE_H_
