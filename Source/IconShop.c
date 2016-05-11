#include "IconShop.h"
#include "TrayIdol.h"
#include "PlasRend.h"

LRESULT CALLBACK i_pProc( HWND i_hWnd, UINT i_uMsg, WPARAM i_wParam, LPARAM i_lParam )
{
    if ( i_uMsg == WM_CLOSE ) return 0;

    return DefWindowProc( i_hWnd, i_uMsg, i_wParam, i_lParam );
}

void Move2ndHand( HWND i_hWnd, IS_FRAME *f, POINT *i_pCur )
{
    POINT pSrc = { 0, 0 };
    SIZE  pSiz = { f->_x, f->_y };
    POINT pDst = { i_pCur->x - f->_ix, i_pCur->y - f->_iy };

    BLENDFUNCTION blenda = { AC_SRC_OVER, 0, gm_hTM ? 127 : 255, AC_SRC_ALPHA };
    UpdateLayeredWindow( i_hWnd, 0, &pDst, &pSiz, f->hDC, &pSrc, 0, &blenda, ULW_ALPHA );
    if ( !gm_hTM )  SetWindowPos( i_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW );
}

IS_FACTORY *MakeIS_FACTORY( IS_HEADER *h )
{
    DWORD dErr = ERROR_ICONSHOP( 0x00 );

    IS_FACTORY *f;
    IS_FRAME *o = 0;

    CHECK_ERROR( f = (IS_FACTORY *) MemAlloc( sizeof( IS_FACTORY ) / 4 ) );

    MemZero( sizeof( IS_FACTORY ) / 4, f );
    MemCopy( sizeof( IS_HEADER ) / 4, &f->head, h );

    CHECK_ERROR( o = LoadIS_FRAME( MAKEINTRESOURCE( h->resId ) ) );
    CHECK_ERROR( f->rndy = MakeIS_PLASMA( o, h ) );

    CHECK_ERROR( f->hwnd = CreateClassWindow( &gi_sCN, &gi_sWN, i_pProc, WS_ICONSHOP, WX_ICONSHOP ) );
 //   CHECK_ERROR( f->sync = MakeIS_SYNCHRO( f ) );

    SetWindowPos( f->hwnd, HWND_TOPMOST, 100, 100, 100, 100, SWP_SHOWWINDOW );

    dErr = ERROR_SUCCESS;

CatchError:

    KillIS_FRAME( o );

    if ( dErr )
    {
        KillIS_FACTORY( f );
        f = 0;

        MsgErr( dErr, MB_ICONWARNING );
    }

    return f;
}

void KillIS_FACTORY( IS_FACTORY *f )
{
    if ( f )
    {
//        if ( f->muta ) CloseHandle( f->muta );
//        if ( f->sema ) CloseHandle( f->sema );

        if ( f->hwnd ) DestroyWindow( f->hwnd );
//        if ( f->sync ) KillIS_SYNCHRO( f->sync );

/*        if ( f->drtv )
        {
//            DestroyQueue( f );
//            WaitForSingleObject( f->drtv, INFINITE );
            CloseHandle( f->drtv );
        }
*/
        KillIS_FRAME( f->rndy );
        MemFree( f );
    }
}
