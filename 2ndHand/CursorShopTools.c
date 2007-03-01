#include "CursorShop.h"

void SaveCS_Frame( CS_FRAME *s, LPCSTR fileName )
{
	BITMAPFILEHEADER bmfh = { 0 };
	DWORD written;

	HANDLE file = CreateFile( fileName, GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_ARCHIVE, 0 );

	bmfh.bfType = MAKEWORD( 'B', 'M' );
    bmfh.bfOffBits = sizeof( BITMAPFILEHEADER ) + sizeof( BITMAPINFOHEADER );
    bmfh.bfSize = bmfh.bfOffBits + s->bih.biSizeImage;

	WriteFile( file, &bmfh, sizeof( BITMAPFILEHEADER ), &written, 0 );
	WriteFile( file, &s->bih, sizeof( BITMAPINFOHEADER ), &written, 0 );
	WriteFile( file, s->dPx, s->bih.biSizeImage, &written, 0 );

	CloseHandle( file );
}

void TestRect( RECT *c_rBounds, POINT *c_pHit )
{
	if ( c_rBounds->left   > c_pHit->x ) c_rBounds->left   = c_pHit->x;
	if ( c_rBounds->top    > c_pHit->y ) c_rBounds->top    = c_pHit->y;
	if ( c_rBounds->right  < c_pHit->x ) c_rBounds->right  = c_pHit->x;
	if ( c_rBounds->bottom < c_pHit->y ) c_rBounds->bottom = c_pHit->y;
}
