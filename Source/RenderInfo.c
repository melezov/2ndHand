#include "CursorShop.h"

BOOL MakeCS_RenderInfo( CS_RENDERINFO *f, CS_HEADER *h, CS_BITMAPINFO *o )
{
	BOOL iMake;
	BYTE aaF = h->aaFactor;

	CS_BITMAPCONTEXT *s = &f->show;

	int x, y;

	int oX = _x( o );
	int oY = _y( o );

	int sS = (int) ( h->borderWidth * aaF + 0.5f );
	int s2 = sS << 1;

	iMake = MakeCS_BitmapContext( s, _x( o ) + s2, _y( o ) + s2 );
	if ( iMake ) return ERROR_CURSORSHOP( 0x30 );

	for ( y = s2; y >= 0; y -- )
	for ( x = s2; x >= 0; x -- )
	{
		int dX = x - sS;
		int dY = y - sS;

		if ( dX * dX + dY * dY <= sS * sS )
		{
			if ( !BitBlt( _d( s ), x, y, oX, oY, _d( o ), 0, 0, SRCPAINT ) )
			{
				return ERROR_CURSORSHOP( 0x31 );
			}
		}
	}

	for ( y = _y( s ) - 1; y >= 0; y -- )
	for ( x = _x( s ) - 1; x >= 0; x -- )
	{
		COLORREF *col = &_PX( s, x, y );

		int rX = x - sS;
		int rY = y - sS;

		if ( *col == CS_BLACK ) continue;

		if ( ( rX >= 0 ) && ( rX < oX ) &&
			 ( rY >= 0 ) && ( rY < oY ) )
		{
			COLORREF cur = _PX( o, rX, rY );

			if ( cur != CS_BLACK )
			{
				*col = cur;
				continue;
			}
		}

		*col = CS_SHADOW_OPAQ;
	}

	f->utex = CreateMutex( 0, 0, 0 );
	if ( !f->utex ) return ERROR_CURSORSHOP( 0x32 );

	return ERROR_SUCCESS;
}

BOOL RenderCS_Frame( CS_FRAME *f, CS_BITMAPCONTEXT *o, CS_HEADER *h, WORD rotStep, CS_FRAME *g )
{
	CS_FRAME t;

	const float fi = rotStep * (float) M_PI * 2 / h->rotSteps;

	const BYTE aaF = h->aaFactor;

	const int oX = _x( o );
	const int oY = _y( o );

	const int sB = (int) ( h->borderWidth * aaF + 0.5f ) << 1;

	const int dX = h->destSize.cx * aaF + sB;
	const int dY = h->destSize.cy * aaF + sB;

	const float stepHX = cosf( fi );
	const float stepHY = cosf( M_PI_2 - fi );

	const float stepVX = - stepHY / CS_ASPECT_CORRECTION;
	const float stepVY =   stepHX / CS_ASPECT_CORRECTION;

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

	SIZE size = { (long) ( maxX - minX + 1 ), (long) ( maxY - minY + 1 ) };

	const int sS = (int) ( h->shadowDist * aaF + 0.5f );
	const DWORD sD = ( 1 + size.cx + sS ) * sS;

	int x, y, iMake;

	iMake = MakeCS_Frame( &t, size.cx + sS, size.cy + sS );
	if ( iMake ) return ERROR_CURSORSHOP( 0x33 );

	for ( y = size.cy; y >= 0; y -- )
	for ( x = size.cx; x >= 0; x -- )
	{
		float fX = ( ( x + minX ) - ( y + minY ) * scaleV ) * aspectX;
		float fY = ( ( y + minY ) - ( x + minX ) * scaleH ) * aspectY;

		if ( ( fX >= 0 ) && ( fX < oX ) && ( fY >= 0 ) && ( fY < oY ) )
		{
			COLORREF col = _PX( o, (int) fX, (int) fY );

			if ( col != CS_BLACK )
			{
				COLORREF *cur = &_PX( &t, x, y );

				if ( cur[ sD ] == CS_BLACK )
				{
					cur[ sD ] = ( col == CS_SHADOW_OPAQ ) ? CS_SHADOW_LITE : CS_SHADOW_DARK;

					TestCS_FramePosInfo( &t.fpi, x + sS, y + sS );
				}

				*cur = col;

				TestCS_FramePosInfo( &t.fpi, x, y );
			}
		}
	}

/*	for ( y = t.fpi.rBounds.bottom; y >= t.fpi.rBounds.top; y -- )
	for ( x = t.fpi.rBounds.right; x >= t.fpi.rBounds.left; x -- )
	{
		if ( _PX( &t, x, y ) == CS_BLACK ) _PX( &t, x, y ) = 0xffffff00;
	}
*/
	memcpy( g, &t, sizeof( CS_FRAME ) );

	size.cx = ( t.fpi.rBounds.right - t.fpi.rBounds.left + 1 ) / aaF;
	size.cy = ( t.fpi.rBounds.bottom - t.fpi.rBounds.top + 1 ) / aaF;

	iMake = MakeCS_Frame( f, size.cx, size.cy );

	if ( iMake )
	{
		KillCS_Frame( &t );
		return ERROR_CURSORSHOP( 0x34 );
	}

//	r->fpi.pCenter.x = (long) ( ( f->cols[ 0 ].pnt.x * x1 + f->cols[ 0 ].pnt.y * x2 - minX + sB - rBounds.left ) / aaF );
//	r->fpi.pCenter.y = (long) ( ( f->cols[ 0 ].pnt.x * y1 + f->cols[ 0 ].pnt.y * y2 - minY + sB - rBounds.top  ) / aaF );

	for ( y = size.cy - 1; y >= 0; y -- )
	for ( x = size.cx - 1; x >= 0; x -- )
	{
		BYTE a, b, c;
		DWORD argb[ 4 ] = { 0 };

		DWORD *dSeek = &_PX( &t, x * aaF + t.fpi.rBounds.left, y * aaF + t.fpi.rBounds.top );

		for ( b = 0; b < aaF; b ++ )
		for ( a = 0; a < aaF; a ++ )
		{
			BYTE *bSeek = (BYTE *) &dSeek[ a + b * _x( &t ) ];

			if ( * (DWORD *) bSeek )
			{
				for ( c = 0; c < 4; c ++ )
				{
					argb[ c ] += bSeek[ c ];
				}
			}
		}

		if ( argb[ 3 ] )
		{
			BYTE *bSeek = (BYTE *) &_PX( f, x, y );

			for ( c = 0; c < 4; c ++ )
			{
				bSeek[ c ] = (BYTE) ( argb[ c ] / ( aaF * aaF ) );
			}
		}
	}

//	KillCS_Frame( &w );
//	outtyText( "Done", rotStep );

	return ERROR_SUCCESS;
}

BOOL KillCS_RenderInfo( CS_RENDERINFO *f )
{
	BOOL iKill = KillCS_BitmapContext( &f->show );

	iKill |= !CloseHandle( f->utex );

	return iKill ? ERROR_CURSORSHOP( 0x39 ) : ERROR_SUCCESS;
}
