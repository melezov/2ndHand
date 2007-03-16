#include "CursorShop.h"

BOOL MakeCS_RenderInfo( CS_RENDERINFO *f, CS_HEADER *h, CS_FRAME *o )
{
	BOOL iMake;
	BYTE aaF = h->aaFactor;

	CS_BITMAPCONTEXT *s = &f->show.bcx;

	int x, y;

	int oX = _x( o );
	int oY = _y( o );

	int sS = (int) ( h->borderWidth * aaF + 0.5f );
	int s2 = sS << 1;

	iMake = MakeCS_BitmapContext( s, oX + s2, oY + s2 );
	if ( iMake ) return ERROR_CURSORSHOP( 0x30 );

	for ( y = s2; y >= 0; y -- )
	for ( x = s2; x >= 0; x -- )
	{
		int dX = x - sS;
		int dY = y - sS;

		if ( dX * dX + dY * dY > sS * sS ) continue;

		if ( !BitBlt( _d( s ), x, y, oX, oY, _d( o ), 0, 0, SRCPAINT ) ) return ERROR_CURSORSHOP( 0x31 );
	}

	for ( y = _y( s ) - 1; y >= 0; y -- )
	for ( x = _x( s ) - 1; x >= 0; x -- )
	{
		COLORREF col, *cur;

		int rX = x - sS;
		int rY = y - sS;

		cur = &_PX( s, x, y );

		if ( ! *cur ) continue;

		*cur = ( rX >= 0 ) && ( rX < oX ) &&
			   ( rY >= 0 ) && ( rY < oY ) &&

			   ( ( col = _PX( o, rX, rY ) ) ) ? col : CS_SHADOW_OPAQ;
	}

	_cx( &f->show ) = _cx( o ) + sS;
	_cy( &f->show ) = _cy( o ) + sS;

	return ERROR_SUCCESS;
}

BOOL RotateCS_Frame( CS_FRAME *f, CS_FRAME *o, CS_HEADER *h, float fi, float rr )
{
	const BYTE aaF = h->aaFactor;

	const int oX = _x( o );
	const int oY = _y( o );

	const int sB = (int) ( h->borderWidth * aaF + 0.5f ) << 1;

	const int dX = h->destSize.cx * aaF + sB;
	const int dY = h->destSize.cy * aaF + sB;

	const float stepOX = cosf( fi );
	const float stepOY = cosf( M_PI_2 - fi );

	const float stepHX = stepOX * rr;
	const float stepHY = stepOY * rr;

	const float stepVX = - stepOY / CS_ASPECT_CORRECTION;
	const float stepVY =   stepOX / CS_ASPECT_CORRECTION;

	const float x1 = dX * stepHX;
	const float y1 = dX * stepHY;
	const float x2 = dY * stepVX;
	const float y2 = dY * stepVY;

	const float minX = min( min( 0, x1 ), min( x2, x1 + x2 ) );
	const float minY = min( min( 0, y1 ), min( y2, y1 + y2 ) );
	const float maxX = max( max( 0, x1 ), max( x2, x1 + x2 ) );
	const float maxY = max( max( 0, y1 ), max( y2, y1 + y2 ) );

	const float scaleV = stepVX / stepVY;
	const float scaleH = stepHY / stepHX;

	const float aspectX = _x( o ) / ( dX * ( stepHX - stepHY * scaleV ) );
	const float aspectY = _y( o ) / ( dY * ( stepVY - stepVX * scaleH ) );

	int wX = (int) ( maxX - minX + 1 );
	int wY = (int) ( maxY - minY + 1 );

	const int sS = (int) ( h->shadowDist * aaF + 0.5f );
	const DWORD sD = ( 1 + wX + sS ) * sS;

	int x, y, iMake;

	iMake = MakeCS_Frame( f, wX + sS, wY + sS );
	if ( iMake ) return ERROR_CURSORSHOP( 0x40 );

	wX --;
	wY --;

	for ( y = wY; y >= 0; y -- )
	for ( x = wX; x >= 0; x -- )
	{
		float fX = ( ( x + minX ) - ( y + minY ) * scaleV ) * aspectX;
		float fY = ( ( y + minY ) - ( x + minX ) * scaleH ) * aspectY;

		COLORREF col, *cur;

		if ( ( fX < 0 ) || ( fX >= oX ) ) continue;
		if ( ( fY < 0 ) || ( fY >= oY ) ) continue;

		col = _PX( o, (int) fX, (int) fY );
		if ( ! col ) continue;

		cur = &_PX( f, x, y );

		if ( ! cur[ sD ] )
		{
			cur[ sD ] = ( col == CS_SHADOW_OPAQ ) ? CS_SHADOW_LITE : CS_SHADOW_DARK;
			TestCS_FramePosInfo( &f->fpi, x + sS, y + sS );
		}

		*cur = col;
		TestCS_FramePosInfo( &f->fpi, x, y );
	}

	_cx( f ) = ( _cx( o ) * x1 / oX + _cy( o ) * x2 / oY - minX );
	_cy( f ) = ( _cx( o ) * y1 / oX + _cy( o ) * y2 / oY - minY );

	_rl( f ) -= ( ( aaF - flour( _cx( f ) - _rl( f ) ) ) % aaF + aaF ) % aaF;
	_rt( f ) -= ( ( aaF - flour( _cy( f ) - _rt( f ) ) ) % aaF + aaF ) % aaF;

	_rr( f ) = ( ( _rr( f ) - _rl( f ) ) / aaF + 1 ) * aaF + _rl( f ) - 1;
	_rb( f ) = ( ( _rb( f ) - _rt( f ) ) / aaF + 1 ) * aaF + _rt( f ) - 1;

	return ERROR_SUCCESS;
}

BOOL ShrinkCS_Frame( CS_FRAME *f, CS_FRAME *o, BYTE aaF )
{
	int lX = _x( o ) - 1;
	int lY = _y( o ) - 1;

	int wX = ( _rr( o ) - _rl( o ) + 1 ) / aaF;
	int wY = ( _rb( o ) - _rt( o ) + 1 ) / aaF;

	int x, y;

	BOOL iMake = MakeCS_Frame( f, wX, wY );
	if ( iMake ) return ERROR_CURSORSHOP( 0x42 );

	wX --;
	wY --;

	for ( y = wY; y >= 0; y -- )
	for ( x = wX; x >= 0; x -- )
	{
		DWORD argb[ 4 ] = { 0 };
		int a, b, c;

		int sX = x * aaF + _rl( o );
		int sY = y * aaF + _rt( o );

		int eX = sX + aaF - 1;
		int eY = sY + aaF - 1;

		if ( sX <  0 ) sX =  0;
		if ( sY <  0 ) sY =  0;

		if ( eX > lX ) eX = lX;
		if ( eY > lY ) eY = lY;

		for ( b = eY; b >= sY; b -- )
		for ( a = eX; a >= sX; a -- )
		{
			BYTE *bPx = (BYTE *) &_PX( o, a, b );

			if ( * (DWORD *) bPx )
			{
				for ( c = 0; c < 4; c ++ )
				{
					argb[ c ] += bPx[ c ];
				}
			}
		}

		if ( argb[ 3 ] )
		{
			BYTE *bPx = (BYTE *) &_PX( f, x, y );

			for ( c = 0; c < 4; c ++ )
			{
				bPx[ c ] = (BYTE) ( argb[ c ] / ( aaF * aaF ) );
			}
		}
	}

	_ix( f ) = flour( ( _cx( o ) - _rl( o ) ) / aaF );
	_iy( f ) = flour( ( _cy( o ) - _rt( o ) ) / aaF );

	return ERROR_SUCCESS;
}

BOOL RenderCS_Frame( CS_FRAME *f, CS_FACTORY *o, float fi, float rr )
{
	BOOL iMake;
	CS_FRAME g;

	iMake = RotateCS_Frame( &g, &o->rndy.show, &o->head, fi, rr );
	if ( iMake ) return ERROR_CURSORSHOP( 0x41 );

	iMake = ShrinkCS_Frame( f, &g, o->head.aaFactor );
	if ( iMake ) return ERROR_CURSORSHOP( 0x42 );

	iMake = KillCS_Frame( &g );
	if ( iMake ) return ERROR_CURSORSHOP( 0x43 );

	return ERROR_SUCCESS;
}

BOOL KillCS_RenderInfo( CS_RENDERINFO *f )
{
	BOOL iKill = KillCS_Frame( &f->show );

	return iKill ? ERROR_CURSORSHOP( 0x39 ) : ERROR_SUCCESS;
}
