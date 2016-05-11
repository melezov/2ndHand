#include "SharCode.h"
#include "MainGxUI.h"

void SharCode()
{
    DWORD dErr = ERROR_SHARCODE( 0x00 );
    HANDLE hMux;

    CHECK_ERROR( hMux = CreateMutex( 0, TRUE, &gs_sSH ) );
    CHECK_ERROR( GetLastError() != ERROR_ALREADY_EXISTS );
    CHECK_ERROR( !CoGetMalloc( 1, &gs_vMem ) );

    dErr = MainGxUI();

CatchError:

    if ( hMux ) CloseHandle( hMux );

    ExitProcess( MsgErr( dErr, MB_ICONERROR ) );
}

DWORD MsgErr( DWORD s_dErr, DWORD s_dIcon )
{
    if ( s_dErr )
    {
        TCHAR mBuf[ MAX_STRING ];
        TCHAR eBuf[ MAX_STRING ];

        wsprintf( eBuf, TEXT( "Error in %s!" ), &gs_sSH );
        wsprintf( mBuf, TEXT( "%s an error code:\n\"0x%08X\"" ),
            ( s_dIcon == MB_ICONERROR ) ? TEXT( "Exiting with" ) : TEXT( "Encountered" ), s_dErr );

        SetLastError( s_dErr );
        MessageBox( 0, mBuf, eBuf, s_dIcon | MB_OK );
    }

    return s_dErr;
}

HWND CreateClassWindow( COAL *s_sCN, COAL *s_sWN, WNDPROC s_pProc, DWORD s_dStyle, DWORD s_dStyleEx )
{
    TCHAR wBuf[ MAX_STRING ];

    WNDCLASSEX xWCX;
    MemZero( sizeof( WNDCLASSEX ) / 4, &xWCX );

    xWCX.cbSize = sizeof( WNDCLASSEX );
    xWCX.hInstance = gs_hInst;
    xWCX.lpszClassName = s_sCN;
    xWCX.lpfnWndProc = s_pProc;
    xWCX.hCursor = LoadCursor( 0, IDC_ARROW );
    xWCX.hIcon = LoadIcon( gs_hInst, MAKEINTRESOURCE( IDI_POINTER ) );

    if ( !RegisterClassEx( &xWCX ) && ( GetLastError() != ERROR_CLASS_ALREADY_EXISTS ) ) return 0;

    wsprintf( wBuf, TEXT( "%s - %s Window" ), &gs_sSH, s_sWN );
    return CreateWindowEx( s_dStyleEx, s_sCN, wBuf, s_dStyle, 0, 0, 0, 0, 0, 0, 0, 0 );
}
