#include "FramServ.h"

IS_FRAME *MakeIS_FRAME( int f_iX, int f_iY )
{
    DWORD dErr = ERROR_FRAMSERV( 0x00 );
    IS_FRAME *f = 0;

    CHECK_ERROR( ( f_iX > 0 ) && ( f_iY > 0 ) );

    CHECK_ERROR( f = (IS_FRAME *) MemAlloc( sizeof( IS_FRAME ) / 4 ) );
    MemZero( sizeof( IS_FRAME ) / 4, f );

    f->bih.biSize = sizeof( BITMAPINFOHEADER );
    f->bih.biPlanes = 1;
    f->bih.biBitCount = 32;
    f->bih.biSizeImage = f_iX * f_iY << 2;

    f->_x =   ( f->_rl = f_iX );
    f->_y = - ( f->_rt = f_iY );

    CHECK_ERROR( f->hBM = CreateDIBSection( 0, (BITMAPINFO *) &f->bih, 0, &f->dPx, 0, 0 ) );
    f->_y = f_iY;

    CHECK_ERROR( f->hDC = CreateCompatibleDC( 0 ) );
    CHECK_ERROR( f->hGO = SelectObject( f->hDC, f->hBM ) );

    dErr = ERROR_SUCCESS;

CatchError:

    if ( dErr )
    {
        KillIS_FRAME( f );
        f = 0;

        MsgErr( dErr, MB_ICONWARNING );
    }

    return f;
}

IS_FRAME *LoadIS_FRAME( COAL *path )
{
    DWORD dErr = ERROR_FRAMSERV( 0x10 );
    IS_FRAME *f = 0, *o;

    DWORD oRes = (DWORD) path <= IDX_LAST;

    CHECK_ERROR( o = (IS_FRAME *) MemAlloc( sizeof( IS_FRAME ) / 4 ) );
    MemZero( sizeof( IS_FRAME ) / 4, o );

    CHECK_ERROR( o->hBM = LoadImage( oRes ? gs_hInst : 0, path, IMAGE_BITMAP, 0, 0, oRes ? 0 : LR_LOADFROMFILE ) );
    CHECK_ERROR( GetObject( o->hBM, sizeof( BITMAP ), &o->bih ) );
    CHECK_ERROR( o->hDC = CreateCompatibleDC( 0 ) );
    CHECK_ERROR( o->hGO = SelectObject( o->hDC, o->hBM ) );

    CHECK_ERROR( f = MakeIS_FRAME( o->_x, o->_y ) );
    CHECK_ERROR( BitBlt( f->hDC, 0, 0, o->_x, o->_y, o->hDC, 0, 0, SRCCOPY ) );

    dErr = ERROR_SUCCESS;

CatchError:

    KillIS_FRAME( o );

    if ( dErr )
    {
        KillIS_FRAME( f );
        f = 0;
    }

    return f;
}

#define S_BFH sizeof( BITMAPFILEHEADER )
#define S_BIH sizeof( BITMAPINFOHEADER )

BOOL SaveIS_FRAME( IS_FRAME *f, COAL *path )
{
    DWORD dErr = ERROR_FRAMSERV( 0x20 );

    int i;
    DWORD dWrt;
    HANDLE hFil;

    BITMAPFILEHEADER bmfh = { MAKEWORD( 'B', 'M' ), S_BFH + S_BIH + f->bih.biSizeImage, 0, 0, S_BFH + S_BIH };

    CHECK_ERROR( ( hFil = CreateFile( path, GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_ARCHIVE, 0 ) ) != INVALID_HANDLE_VALUE );

    CHECK_ERROR( WriteFile( hFil, &bmfh, S_BFH, &dWrt, 0 ) && ( dWrt == S_BFH ) );
    CHECK_ERROR( WriteFile( hFil, &f->bih, S_BIH, &dWrt, 0 ) && ( dWrt == S_BIH ) );

    for ( i = f->_y - 1; i >= 0; i -- )
    {
        CHECK_ERROR( WriteFile( hFil, f->dPx + f->_x * i, f->_x << 2, &dWrt, 0 ) && ( dWrt == f->_x << 2 ) );
    }

    dErr = ERROR_SUCCESS;

CatchError:

    if ( hFil ) CloseHandle( hFil );

    return MsgErr( dErr, MB_ICONINFORMATION );
}

#undef S_BIH
#undef S_BFH

void KillIS_FRAME( IS_FRAME *f )
{
    if ( f )
    {
        if ( f->hDC )
        {
            if ( f->hGO ) SelectObject( f->hDC, f->hGO );
            DeleteDC( f->hDC );
        }

        if ( f->hBM ) DeleteObject( f->hBM );

        MemFree( f );
    }
}

void TestIS_FRAME( IS_FRAME *f, int t_iX, int t_iY )
{
    if ( f->_rr < t_iX ) f->_rr = t_iX;
    if ( f->_rb < t_iY ) f->_rb = t_iY;
    if ( f->_rl > t_iX ) f->_rl = t_iX;
    if ( f->_rt > t_iY ) f->_rt = t_iY;
}
