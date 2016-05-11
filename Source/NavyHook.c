#include "NavyHook.h"

LRESULT CALLBACK RKsHookProc( int m_nCode, WPARAM m_wParam, LPARAM m_lParam )
{
    #define xKLHS ( (KBDLLHOOKSTRUCT *) m_lParam )
    #define xMLHS ( (MSLLHOOKSTRUCT  *) m_lParam )

    switch ( m_wParam )
    {
        case ( WM_KEYDOWN ):
        case ( WM_KEYUP ):
        case ( WM_CHAR ):
        case ( WM_DEADCHAR ):
        case ( WM_SYSKEYDOWN ):
        case ( WM_SYSKEYUP ):
        case ( WM_SYSCHAR ):
        case ( WM_SYSDEADCHAR ): PostMessage
        (
            gm_hWnd, WM_KEYHOOK,

            MAKEWPARAM( m_wParam, LOWORD( xKLHS->flags ) ),
            MAKELPARAM( xKLHS->vkCode, xKLHS->scanCode )
        );
        break;

        case ( WM_MOUSEMOVE ):
        case ( WM_LBUTTONDOWN ):
        case ( WM_LBUTTONUP ):
        case ( WM_LBUTTONDBLCLK ):
        case ( WM_RBUTTONDOWN ):
        case ( WM_RBUTTONUP ):
        case ( WM_RBUTTONDBLCLK ):
        case ( WM_MBUTTONDOWN ):
        case ( WM_MBUTTONUP ):
        case ( WM_MBUTTONDBLCLK ):
        case ( WM_MOUSEWHEEL ): PostMessage
        (
            gm_hWnd, WM_RATHOOK,

            MAKEWPARAM( m_wParam, HIWORD( xMLHS->mouseData ) ),
            MAKELPARAM( xMLHS->pt.x, xMLHS->pt.y )
        );
    }

    #undef xMLHS
    #undef xKLHS

    return CallNextHookEx( 0, m_nCode, m_wParam, m_lParam );
}

DWORD SetHooks( DWORD m_oAction )
{
    DWORD dErr = m_oAction ? ERROR_NAVYHOOK( 0x00 ) : ERROR_NAVYHOOK( 0x02 );
    int i;

    for ( i = 1; i >= 0; i -- )
    {
        HHOOK *hCur = &gm_hRKs + i;

        if ( m_oAction )
        {
            CHECK_ERROR( *hCur = SetWindowsHookEx( WH_KEYBOARD_LL + i, RKsHookProc, gs_hInst, 0 ) );
        }
        else if ( *hCur )
        {
            CHECK_ERROR( UnhookWindowsHookEx( *hCur ) );

            *hCur = 0;
        }
    }

    dErr = ERROR_SUCCESS;

CatchError:

    return MsgErr( dErr, MB_ICONWARNING );
}
