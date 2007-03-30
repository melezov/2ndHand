#include "SharedCode.h"

HWND CreateClassWindow( CTRING s_sCN, CTRING s_sWN, WNDPROC s_pProc, DWORD s_dStyle, DWORD s_dStyleEx )
{
    WNDCLASSEX s_xWCX;
    MemZero( sizeof( WNDCLASSEX ) / 4, &s_xWCX );

    s_xWCX.cbSize = sizeof( WNDCLASSEX );
    s_xWCX.hInstance = gm_hInst;
    s_xWCX.lpszClassName = s_sCN;
    s_xWCX.lpfnWndProc = s_pProc;
    s_xWCX.hCursor = LoadCursor( 0, IDC_ARROW );
    s_xWCX.hIcon = LoadIcon( gm_hInst, MAKEINTRESOURCE( IDI_POINTER ) );

    if ( !RegisterClassEx( &s_xWCX ) && ( GetLastError() != ERROR_CLASS_ALREADY_EXISTS ) ) return 0;

    return CreateWindowEx( s_dStyleEx, s_sCN, s_sWN, s_dStyle, 0, 0, 0, 0, 0, 0, 0, 0 );
}

BOOL ERROR_MESSAGE( BOOL x )
{
    MessageBox( 0, 0, 0, 0 );
    return x;
}
