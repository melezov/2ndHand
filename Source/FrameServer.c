#include "CursorShop.h"

BOOL MakeCS_BitmapInfo( CS_BITMAPINFO *f, int f_iX, int f_iY )
{
	memset( f, 0, sizeof( CS_BITMAPINFO ) );

	f->bih.biSize = sizeof( BITMAPINFOHEADER );
	f->bih.biPlanes = 1;
	f->bih.biBitCount = 32;
	f->bih.biWidth = f_iX;
	f->bih.biHeight = f_iY;
	f->bih.biSizeImage = f_iX * f_iY << 2;

	f->hBM = CreateDIBSection( 0, (BITMAPINFO *) &f->bih, 0, &f->dPx, 0, 0 );
	return !f->hBM ? ERROR_CURSORSHOP( 0 ) : ERROR_SUCCESS;
}

BOOL KillCS_BitmapInfo( CS_BITMAPINFO *f )
{
	BOOL iKill = DeleteObject( f->hBM );
	memset( f, 0, sizeof( CS_BITMAPINFO ) );
	return !iKill ? ERROR_CURSORSHOP( 1 ) : ERROR_SUCCESS;
}

BOOL MakeCS_DCInfo( CS_DCINFO *f, HBITMAP h )
{
	memset( f, 0, sizeof( CS_DCINFO ) );

	f->hDC = CreateCompatibleDC( 0 );
	if ( !f->hDC ) return ERROR_CURSORSHOP( 2 );

	f->hGO = SelectObject( f->hDC, h );
	if ( !f->hGO ) KillCS_DCInfo( f );

	return !f->hDC ? ERROR_CURSORSHOP( 3 ) : ERROR_SUCCESS;
}

BOOL KillCS_DCInfo( CS_DCINFO *f )
{
	if ( f->hGO ) SelectObject( f->hDC, f->hGO );
	if ( !DeleteDC( f->hDC ) ) return ERROR_CURSORSHOP( 4 );

	memset( f, 0, sizeof( CS_DCINFO ) );
	return ERROR_SUCCESS;
}

BOOL MakeCS_BitmapContext( CS_BITMAPCONTEXT *f, int f_iX, int f_iY )
{
	BOOL iMake = MakeCS_BitmapInfo( &f->bmi, f_iX, f_iY );
	if ( iMake ) return iMake;

	iMake = MakeCS_DCInfo( &f->dci, f->bmi.hBM );
	if ( iMake ) KillCS_BitmapInfo( &f->bmi );

	return iMake ? ERROR_CURSORSHOP( 5 ) : ERROR_SUCCESS;
}

BOOL KillCS_BitmapContext( CS_BITMAPCONTEXT *f )
{
	BOOL iMake = KillCS_DCInfo( &f->dci ) | KillCS_BitmapInfo( &f->bmi );
	return iMake ? ERROR_CURSORSHOP( 6 ) : ERROR_SUCCESS;
}

BOOL MakeCS_Frame( CS_FRAME *f, int f_iX, int f_iY )
{
	memset( &f->fpi, 0, sizeof( CS_FRAMEPOSINFO ) );
	return MakeCS_BitmapContext( &f->bcx, f_iX, f_iY );
}

BOOL KillCS_Frame( CS_FRAME *f )
{
	BOOL iKill;
	memset( &f->fpi, 0, sizeof( CS_FRAMEPOSINFO ) );
	if ( iKill = KillCS_BitmapContext( &f->bcx ) ) return iKill;
	return ERROR_SUCCESS;
}

BOOL MakeCS_Synhro( CS_SYNHRO *f )
{
	memset( f, 0, sizeof( CS_SYNHRO ) );

	f->status = CS_SYNHRO_IDLE;
	f->sema = CreateEvent( 0, 0, 0, 0 );

	return !f->sema ? ERROR_CURSORSHOP( 6 ) : ERROR_SUCCESS;
}

BOOL KillCS_Synhro( CS_SYNHRO *f )
{
	if ( !f->sema || !CloseHandle( f->sema ) ) return ERROR_CURSORSHOP( 7 );

	memset( f, 0, sizeof( CS_SYNHRO ) );
	return ERROR_SUCCESS;
}