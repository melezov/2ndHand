#include "CursorShop.h"

CTRING gc_sCN = TEXT( "2NDHAND_CURSOR_CLASS" );
CTRING gc_sWN = TEXT( "2ndHand - Cursor window" );

LRESULT CALLBACK c_pProc( HWND c_hWnd, UINT c_uMsg, WPARAM c_wParam, LPARAM c_lParam )
{
    return DefWindowProc( c_hWnd, c_uMsg, c_wParam, c_lParam );
}

void KillCS_Factory( CS_FACTORY *f )
{
    if ( f )
    {
        if ( f->muta ) CloseHandle( f->muta );
        if ( f->sema ) CloseHandle( f->sema );

        if ( f->hwnd ) DestroyWindow( f->hwnd );


        if ( f->drtv )
        {
            DestroyQueue( f );
            WaitForSingleObject( f->drtv, INFINITE );
            CloseHandle( f->drtv );
        }

        KillCS_Frame( f->rndy );
        MemFree( f );
    }
}

DWORD WINAPI WorkerThread( CS_FACTORY *f );

CS_FACTORY *MakeCS_Factory( CS_HEADER *h )
{
	BYTE iMake = 0xF0;
    CS_FRAME *o = 0;

    CS_FACTORY *f = (CS_FACTORY *) MemAlloc( sizeof( CS_FACTORY ) / 4 );
    if ( !f ) goto MakeError; iMake ++;

    MemZero( sizeof( CS_FACTORY ) / 4, f );
    MemCopy( sizeof( CS_HEADER ) / 4, &f->head, h );

    f->sema = CreateEvent( 0, 0, 0, 0 );
    if ( !f->sema ) goto MakeError; iMake ++;

    f->muta = CreateMutex( 0, 0, 0 );
    if ( !f->muta ) goto MakeError; iMake ++;

    f->reqs = 0;
    f->swdn = 0;

    o = LoadCS_Frame( h->resId );
    if ( !o ) goto MakeError; iMake ++;

    f->rndy = PlsmCS_Frame( o, h );
    if ( !f->rndy ) goto MakeError; iMake ++;

    f->drtv = CreateThread( 0, 0, (LPTHREAD_START_ROUTINE) WorkerThread, f, 0, 0 );
    if ( !f->drtv ) goto MakeError; iMake ++;

    f->hwnd = CreateClassWindow( gc_sCN, gc_sWN, c_pProc, WS_CURSORSHOP, WX_CURSORSHOP );
    if ( !f->hwnd ) goto MakeError; iMake ++;

    KillCS_Frame( o );

    return f;

MakeError:

    MessageBox( gm_hWnd, TEXT( "MakeCS_Factory crash!" ), 0, MB_ICONHAND );

    KillCS_Frame( o );
    KillCS_Factory( f );

    SetLastError( ERROR_CURSORSHOP( iMake ) );
    return 0;
}
