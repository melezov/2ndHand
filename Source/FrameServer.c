#include "CursorShop.h"

CS_FRAME *MakeCS_Frame( int f_iX, int f_iY )
{
    BYTE iMake = 0x00;

    CS_FRAME *f = (CS_FRAME *) MemAlloc( sizeof( CS_FRAME ) / 4 );
    if ( !f ) goto MakeError; iMake ++;

    MemZero( sizeof( CS_FRAME ) / 4, f );

	f->bih.biSize = sizeof( BITMAPINFOHEADER );
	f->bih.biPlanes = 1;
	f->bih.biBitCount = 32;
	f->bih.biSizeImage = f_iX * f_iY << 2;

	f->_x =   ( f->_rl = f_iX );
	f->_y = - ( f->_rt = f_iY );

    f->hBM = CreateDIBSection( 0, (BITMAPINFO *) &f->bih, 0, &f->dPx, 0, 0 );
    if ( !f->hBM ) goto MakeError; iMake ++;

    f->_y = f_iY;

    f->hDC = CreateCompatibleDC( 0 );
    if ( !f->hDC ) goto MakeError; iMake ++;

    f->hGO = SelectObject( f->hDC, f->hBM );
    if ( !f->hGO ) goto MakeError;

    return f;

MakeError:

    MessageBox( 0, TEXT( "MakeCS_Frame crash!" ), 0, MB_ICONHAND );

    KillCS_Frame( f );
    SetLastError( ERROR_CURSORSHOP( iMake ) );
    return 0;
}

CS_FRAME *LoadCS_Frame( CTRING path )
{
    BYTE iMake = 0x10;
    CS_FRAME *f = 0;

    CS_FRAME *o = (CS_FRAME *) MemAlloc( sizeof( CS_FRAME ) / 4 );
    if ( !o ) goto MakeError; iMake ++;

    MemZero( sizeof( CS_FRAME ) / 4, o );

    o->hBM = (HBITMAP) LoadImage( 0, path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );
    if ( !o->hBM ) goto MakeError; iMake ++;

    if ( !GetObject( o->hBM, sizeof( BITMAP ), &o->bih ) ) goto MakeError; iMake ++;

    o->hDC = CreateCompatibleDC( 0 );
    if ( !o->hDC ) goto MakeError; iMake ++;

    o->hGO = SelectObject( o->hDC, o->hBM );
    if ( !o->hGO ) goto MakeError;

    f = MakeCS_Frame( o->_x, o->_y );
    if ( !f ) goto MakeError; iMake ++;

    if ( !BitBlt( f->hDC, 0, 0, o->_x, o->_y, o->hDC, 0, 0, SRCCOPY ) ) goto MakeError; iMake ++;

    KillCS_Frame( o );

    return f;

MakeError:

    MessageBox( 0, TEXT( "LoadCS_Frame crash!" ), 0, MB_ICONHAND );

    KillCS_Frame( o );
    KillCS_Frame( f );

    SetLastError( ERROR_CURSORSHOP( iMake ) );
    return 0;
}

#define BITMAP_LENGTH ( sizeof( BITMAPFILEHEADER ) + sizeof( BITMAPINFOHEADER ) )

BOOL SaveCS_Frame( CS_FRAME *f, CTRING path )
{
    DWORD i;
    BYTE iMake = 0x20;

    BITMAPFILEHEADER bmfh;
    BITMAPINFOHEADER bmih;

    void *schedule[ 3 ];
    DWORD sizes[ 3 ];

    HANDLE file;

    schedule[ 0 ] = &bmfh;
    schedule[ 1 ] = &bmih;
    schedule[ 2 ] = f->dPx;

    sizes[ 0 ] = sizeof( BITMAPFILEHEADER );
    sizes[ 1 ] = sizeof( BITMAPINFOHEADER );
    sizes[ 2 ] = f->bih.biSizeImage;

	file = CreateFile( path, GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_ARCHIVE, 0 );
    if ( file == INVALID_HANDLE_VALUE ) goto MakeError; iMake ++;

    bmfh.bfType = MAKEWORD( 'B', 'M' );
    bmfh.bfSize = BITMAP_LENGTH + sizes[ 2 ];
    bmfh.bfReserved1 = bmfh.bfReserved2 = 0;
    bmfh.bfOffBits = BITMAP_LENGTH;

    MemCopy( sizeof( BITMAPINFOHEADER ) / 4, &bmih, &f->bih );
    bmih.biHeight = -bmih.biHeight;

    for ( i = 0; i < 3; i ++ )
    {
        DWORD written;
        BOOL iWrite = WriteFile( file, schedule[ i ], sizes[ i ], &written, 0 );

        if ( !iWrite || ( written != sizes[ i ] ) ) goto MakeError; iMake ++;
    }

    CloseHandle( file );
    return ERROR_SUCCESS;

MakeError:

    MessageBox( 0, TEXT( "SaveCS_Frame crash!" ), 0, MB_ICONHAND );
    SetLastError( ERROR_CURSORSHOP( iMake ) );
    return ERROR_CURSORSHOP( iMake );
}

void KillCS_Frame( CS_FRAME *f )
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

void TestCS_Frame( CS_FRAME *f, int t_iX, int t_iY )
{
	if ( f->_rr < t_iX ) f->_rr = t_iX;
	if ( f->_rb < t_iY ) f->_rb = t_iY;
	if ( f->_rl > t_iX ) f->_rl = t_iX;
	if ( f->_rt > t_iY ) f->_rt = t_iY;
}
