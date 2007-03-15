#include "CursorShop.h"

BOOL MakeCS_Factory( CS_FACTORY **pf, CS_HEADER *h )
{
	POINT ppp;

	CS_FACTORY *f;
	int iMake;

	HBITMAP resHBM = (HBITMAP) LoadImage( 0, h->resId, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );
	if ( !resHBM ) return ERROR_CURSORSHOP( 0x10 );

	*pf = f = VirtualAlloc( 0, sizeof( CS_FACTORY ) + sizeof( CS_FRAME ) * h->rotSteps, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE );
	if ( !f ) return ERROR_CURSORSHOP( 0x11 );

	iMake = MakeCS_OriginInfo( &f->orgy, resHBM );
	if ( iMake ) return iMake;

	iMake = MakeCS_PlasmaInfo( &f->plsy, h, &f->orgy.crop.bmi );
	if ( iMake ) return iMake;

	iMake = MakeCS_RenderInfo( &f->rndy, h, &f->plsy.flop.bmi );
	if ( iMake ) return iMake;

	GetCursorPos( &ppp );
	iMake = ( ( ppp.x + ppp.y ) >> 1 ) % h->rotSteps;
	iMake = RenderCS_Frame( &f->rots[ 0 ].preview, &f->rndy.show, h, (WORD) iMake, &f->rots[ 1 ].preview );

	memcpy( &f->head, h, sizeof( CS_HEADER ) );

	return ERROR_SUCCESS;
}

BOOL KillCS_Factory( CS_FACTORY **pf )
{
	int i;

	CS_FACTORY *f = *pf;

	KillCS_OriginInfo( &f->orgy );
	KillCS_PlasmaInfo( &f->plsy );
	KillCS_RenderInfo( &f->rndy );

	*pf = 0;

	for ( i = 0; i < 2; i ++ )
	{
		KillCS_Frame( &f->rots[ i ].preview );
	}

	return !VirtualFree( f, 0, MEM_RELEASE ) ? ERROR_CURSORSHOP( 0x13 ) : ERROR_SUCCESS;
}

/*
void SaveCS_Frame( CS_FRAME *s, CTRING fileName )
{
	BITMAPFILEHEADER bmfh = { 0 };
	DWORD written;

	HANDLE file = CreateFile( fileName, GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_ARCHIVE, 0 );

	bmfh.bfType = MAKEWORD( 'B', 'M' );
    bmfh.bfOffBits = sizeof( BITMAPFILEHEADER ) + sizeof( BITMAPINFOHEADER );
	bmfh.bfSize = bmfh.bfOffBits + s->bmi.bih.biSizeImage;

	WriteFile( file, &bmfh, sizeof( BITMAPFILEHEADER ), &written, 0 );
	WriteFile( file, &s->bmi.bih, sizeof( BITMAPINFOHEADER ), &written, 0 );
	WriteFile( file, s->bmi.dPx, s->bmi.bih.biSizeImage, &written, 0 );

	CloseHandle( file );
}
*/
