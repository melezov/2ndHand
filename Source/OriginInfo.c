#include "CursorShop.h"

BOOL EnumCS_FramePosInfo( CS_FRAMEPOSINFO *f, CS_BITMAPINFO *r, SIZE *cs )
{
	int x, y;
	DWORD wCnt = 0;

	int rX = _x( r ) - 1;
	int rY = _y( r ) - 1;

	if ( MakeCS_FramePosInfo( f ) ) return ERROR_CURSORSHOP( 0x14 );

	for ( y = rY; y >= 0; y -- )
	for ( x = rX; x >= 0; x -- )
	{
		COLORREF *col = &_PX( r, x, y );

		if ( *col == CS_BLACK ) continue;

		if ( *col == CS_WHITE )
		{
			f->u.fCenter.x += x;
			f->u.fCenter.y += y;

			*col = CS_BLACK;
			wCnt ++;
		}
		else
		{
			if ( f->rBounds.left   > x ) f->rBounds.left   = x;
			if ( f->rBounds.top    > y ) f->rBounds.top    = y;
			if ( f->rBounds.right  < x ) f->rBounds.right  = x;
			if ( f->rBounds.bottom < y ) f->rBounds.bottom = y;
		}
	}

	cs->cx = f->rBounds.right - f->rBounds.left + 1;
	cs->cy = f->rBounds.bottom - f->rBounds.top + 1;

	if ( ( cs->cx <= 0 ) || ( cs->cy <= 0 ) ) return ERROR_CURSORSHOP( 0x15 );

	if ( wCnt )
	{
		f->u.fCenter.x = ( f->u.fCenter.x / wCnt + 0.5f - f->rBounds.left ) / cs->cx;
		f->u.fCenter.y = ( f->u.fCenter.y / wCnt + 0.5f - f->rBounds.top  ) / cs->cy;
	}

	return ERROR_SUCCESS;
}

BOOL MakeCS_OriginInfo( CS_ORIGININFO *f, HBITMAP h )
{
	BOOL iMake;
	SIZE cs;

	CS_BITMAPCONTEXT *r = &f->rsrc, *o = &f->orig, *c = &f->crop;

	iMake = MakeCS_DCInfo( &r->dci, _h( r ) = h );
	if ( iMake ) return ERROR_CURSORSHOP( 0x10 );

	if ( !GetObject( _h( r ), sizeof( BITMAP ), r ) ) return ERROR_CURSORSHOP( 0x11 );
	r->bmi.bih.biHeight = - r->bmi.bih.biHeight;

	iMake = MakeCS_BitmapContext( o, _x( r ), _y( r ) );
	if ( iMake ) return ERROR_CURSORSHOP( 0x12 );

	if ( !BitBlt( _d( o ), 0, 0, _x( o ), _y( o ), _d( r ), 0, 0, SRCCOPY ) ) return ERROR_CURSORSHOP( 0x13 );

	iMake = EnumCS_FramePosInfo( &f->rfpi, &o->bmi, &cs );
	if ( iMake ) return ERROR_CURSORSHOP( 0x16 );

	iMake = MakeCS_BitmapContext( c, cs.cx, cs.cy );
	if ( iMake ) return ERROR_CURSORSHOP( 0x17 );

	if ( !BitBlt( _d( c ), 0, 0, cs.cx, cs.cy, _d( o ), f->rfpi.rBounds.left, f->rfpi.rBounds.top, SRCCOPY ) ) return ERROR_CURSORSHOP( 0x18 );

	return ERROR_SUCCESS;
}

BOOL KillCS_OriginInfo( CS_ORIGININFO *f )
{
	BOOL iKill = KillCS_BitmapContext( &f->rsrc ) |
				 KillCS_BitmapContext( &f->orig ) |
				 KillCS_BitmapContext( &f->crop );

	return iKill ? ERROR_CURSORSHOP( 0x18 ) : ERROR_SUCCESS;
}
