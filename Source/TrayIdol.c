#include "TrayIdol.h"
#include "NavyHook.h"

DWORD TrayAction( DWORD m_dAction )
{
    NOTIFYICONDATA xNID;
    MemZero( sizeof( NOTIFYICONDATA ) / 4, &xNID );

    xNID.cbSize = sizeof( NOTIFYICONDATA );
    xNID.hWnd = gm_hWnd;
    xNID.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    xNID.uCallbackMessage = WM_TRAYIDOL;
    xNID.hIcon = LoadIcon( gs_hInst, MAKEINTRESOURCE( gm_hAC ? IDI_POINTER : IDI_DENIED ) );

    wsprintf( xNID.szTip, TEXT( "%s - %s" ), &gs_sSH, gm_hAC ? TEXT( "Active" ) : TEXT( "Disabled" ) );

    if ( !Shell_NotifyIcon( m_dAction, &xNID ) ) return ERROR_TRAYIDOL( 0x00 );

    return ERROR_SUCCESS;
}

DWORD TrayParse( UINT m_uMsg, LPARAM m_lParam )
{
    if ( m_uMsg == WM_TRAYIDOL )
    {
        if ( m_lParam == WM_RBUTTONDOWN )
        {
            DWORD dTMR;
            POINT cPos;

            HMENU hTPM = CreatePopupMenu();

            TCHAR mBuf[ MAX_STRING ];
            wsprintf( mBuf, TEXT( "Exit %s" ), &gs_sSH );

            gm_hTM = hTPM;
            AppendMenu( hTPM, MF_STRING, WM_DESTROY, mBuf );

            GetCursorPos( &cPos );
            SetForegroundWindow( gm_hWnd );

            dTMR = TrackPopupMenuEx( hTPM, TS_TRAYIDOL, cPos.x, cPos.y, gm_hWnd, 0 );
            DestroyMenu( hTPM );

            gm_hTM = 0;

            if ( dTMR == WM_DESTROY )
            {
                PostMessage( gm_hWnd, WM_CLOSE, 0, 0 );
            }
        }

        else if ( m_lParam == WM_LBUTTONDBLCLK )
        {
            gm_hAC = !gm_hAC ? gm_hWnd : 0;
            SetHooks( (DWORD) gm_hAC );
            TrayAction( NIM_MODIFY );
        }

        return 0;
    }

    else if ( m_uMsg == WM_CREATE )
    {
        WM_TRAYCREA = RegisterWindowMessage( &gm_sTC );
        gm_hAC = gm_hWnd;

        TrayAction( NIM_ADD );
        SetHooks( 1 );
    }

    else if ( m_uMsg == WM_TRAYCREA )
    {
        TrayAction( NIM_ADD );
    }

    else if ( m_uMsg == WM_DESTROY )
    {
        gm_hAC = 0;
        SetHooks( 0 );
        TrayAction( NIM_DELETE );
    }

    return 1;
}
