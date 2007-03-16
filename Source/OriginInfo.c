#include "CursorShop.h"

BOOL EnumCS_FramePosInfo( CS_FRAME *o )
{
	int x, y;
	DWORD wCnt = 0;

	CS_FRAMEPOSINFO *f = &o->fpi;
	CS_BITMAPINFO *r = &o->bcx.bmi;

	int rX = _x( r ) - 1;
	int rY = _y( r ) - 1;

	for ( y = rY; y >= 0; y -- )
	for ( x = rX; x >= 0; x -- )
	{
		COLORREF *col = &_PX( r, x, y );

		if ( ! *col ) continue;

		if ( *col == CS_WHITE )
		{
			f->u.fCenter.x += x;
			f->u.fCenter.y += y;

			*col = 0;

			wCnt ++;
		}
		else TestCS_FramePosInfo( f, x, y );
	}

	if ( wCnt )
	{
		f->u.fCenter.x /= wCnt;
		f->u.fCenter.y /= wCnt;
	}

	f->u.fCenter.x += 0.5f;
	f->u.fCenter.y += 0.5f;

	return ERROR_SUCCESS;
}

BOOL MakeCS_OriginInfo( CS_ORIGININFO *f, HBITMAP h )
{
	BOOL iMake;

	CS_BITMAPCONTEXT *r = &f->rsrc;
	CS_FRAME *o = &f->orig;

	iMake = MakeCS_DCInfo( &r->dci, _h( r ) = h );
	if ( iMake ) return ERROR_CURSORSHOP( 0x10 );

	if ( !GetObject( _h( r ), sizeof( BITMAP ), r ) ) return ERROR_CURSORSHOP( 0x11 );
	r->bmi.bih.biHeight = - r->bmi.bih.biHeight;

	iMake = MakeCS_Frame( o, _x( r ), _y( r ) );
	if ( iMake ) return ERROR_CURSORSHOP( 0x12 );

	if ( !BitBlt( _d( o ), 0, 0, _x( o ), _y( o ), _d( r ), 0, 0, SRCCOPY ) ) return ERROR_CURSORSHOP( 0x13 );

	iMake = EnumCS_FramePosInfo( o );
	if ( iMake ) return ERROR_CURSORSHOP( 0x16 );

	return ERROR_SUCCESS;
}

BOOL KillCS_OriginInfo( CS_ORIGININFO *f )
{
	BOOL iKill = KillCS_BitmapContext( &f->rsrc ) |
				 KillCS_Frame( &f->orig );

	return iKill ? ERROR_CURSORSHOP( 0x18 ) : ERROR_SUCCESS;
}
