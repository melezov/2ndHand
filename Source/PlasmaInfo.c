#include "CursorShop.h"

BOOL PlasmaCS_ColorInfo( CS_COLORINFO *c, CS_BITMAPINFO *o, DWORD *cNum )
{
	int x, y;

	*cNum = 0;
	memset( c, 0, sizeof( CS_COLORINFO ) * CS_ORIGIN_MAX_COLORS );

	for ( y = _y( o ) - 1; y >= 0; y -- )
	for ( x = _x( o ) - 1; x >= 0; x -- )
	{
		DWORD n;

		CS_COLORINFO *seek = c;
		COLORREF col = _PX( o, x, y );

		if ( !col ) continue;

		for ( n = 0; ; seek ++ )
		{
			if ( !seek->rgb )
			{
				seek->rgb = col;
				seek->cnt = 1;
				break;
			}

			if ( seek->rgb == col )
			{
				seek->cnt ++;
				break;
			}

			if ( ++ n >= CS_ORIGIN_MAX_COLORS ) return ERROR_CURSORSHOP( 0x22 );
		}

		if ( *cNum < n + 1 ) *cNum = n + 1;
	}

	if ( ! *cNum ) return ERROR_CURSORSHOP( 0x23 );

	for ( y =     0; y < (int) *cNum - 2; y ++ )
	for ( x = y + 1; x < (int) *cNum - 1; x ++ ) if ( c[ x ].cnt > c[ y ].cnt )
	{
		CS_COLORINFO c_swap;

		memcpy( &c_swap, &c[ y ], sizeof( CS_COLORINFO ) );
		memcpy( &c[ y ], &c[ x ], sizeof( CS_COLORINFO ) );
		memcpy( &c[ x ], &c_swap, sizeof( CS_COLORINFO ) );
	}

	return ERROR_SUCCESS;
}

BOOL PlasmaCS_ColorRef( COLORREF *f, CS_COLORINFO *c, DWORD cNum )
{
	int i, rgb;

	if ( !cNum ) return ERROR_CURSORSHOP( 0x25 );

	for ( i = 0; i < CS_PLASMA_MAX_COLORS; i ++ )
	{
		if ( cNum >= 2 )
		{
			for ( rgb = 0; rgb < 3; rgb ++ )
			{
				float perc = (float) i / ( CS_PLASMA_MAX_COLORS - 1 );

				float grad = (float) ( (BYTE *) &c[ 0 ].rgb )[ rgb ] * perc +
							 (float) ( (BYTE *) &c[ 1 ].rgb )[ rgb ] * ( 1 - perc );

				if ( grad <   0.0f ) grad =   0.0f; else
				if ( grad > 255.0f ) grad = 255.0f;

				( (BYTE * ) &f[ i ] )[ rgb ] = (BYTE) grad;
			}
		}
		else f[ i ] = c[ 0 ].rgb;
	}

	return ERROR_SUCCESS;
}

const BYTE plasmaIndexes[] =
{
	0, 1, 2, 1, 4, 1,
	2, 1, 2, 3, 2, 5,
	0, 3, 2, 3, 4, 3,
	0, 1, 0, 3, 0, 5,

	0, 1, 4, 5, 0, 0,
	4, 1, 2, 5, 0, 0,
	4, 5, 2, 3, 0, 0,
	0, 5, 4, 3, 0, 0
};

BOOL PlasmaCS_BitmapInfo( CS_BITMAPINFO *p, int grad, int *nr )
{
	int i, r, e, kr[ 8 * 6 ], *cr;
	COLORREF *iPx;

	if ( !nr )
	{
		kr[ 3 ] = 0;
		kr[ 4 ] = 0;
		kr[ 5 ] = _x( p ) - 1;
		kr[ 6 ] = _y( p ) - 1;

		kr[ 0 ] = ( kr[ 2 ] = kr[ 5 ] ) + ( kr[ 1 ] = kr[ 6 ] * ( kr[ 5 ] + 1 ) );

		cr = kr + ( r = 3 );

		for ( e = 0; r >= 0; r -- )
		{
			p->dPx[ kr[ r ] ] = 1 + ( rand() % ( CS_PLASMA_MAX_COLORS - 1 ) );
		}

		goto EndCall;
	}

	iPx = &_PX( p, nr[ 4 ], nr[ 5 ] );

	r = *iPx ? 4 : 0;

	e = ( nr[ 2 ] - nr[ 0 ] < 3 ) && ( nr[ 3 ] - nr[ 1 ] < 3 ) ? 4 : 8;

	for ( i = r * 6; i < e * 6; i ++ ) kr[ i ] = nr[ plasmaIndexes[ i ] ];

	for ( i = 0; r < e; r ++ )
	{
		cr = &kr[ r * 6 ];

		if ( r < 4 )
		{
			COLORREF *vPx = &_PX( p, cr[ 4 ], cr[ 5 ] );

			if ( !*vPx )
			{
				int col = ( _PX( p, cr[ 0 ], cr[ 1 ] ) + _PX( p, cr[ 2 ], cr[ 3 ] ) + 1 ) >> 1;

				if ( grad )
				{
					int gen = rand();
					int out = ( gen >> 1 ) % grad;
					col += gen & 1 ? out : -out;
				}

				if ( col < 1 ) col = 1; else if ( col > CS_PLASMA_MAX_COLORS - 1 ) col = CS_PLASMA_MAX_COLORS - 1;

			   *vPx = col;
			}

			i += *vPx;

			if ( r == 3 ) *iPx = ( i + 2 ) >> 2;
		}

		else if ( ( cr[ 2 ] - cr[ 0 ] > 1 ) || ( cr[ 3 ] - cr[ 1 ] > 1 ) )
		{
EndCall:	cr[ 4 ] = ( cr[ 0 ] + cr[ 2 ] + 1 ) >> 1;
			cr[ 5 ] = ( cr[ 1 ] + cr[ 3 ] + 1 ) >> 1;

			PlasmaCS_BitmapInfo( p, grad >> 1, cr );
		}
	}

	return ERROR_SUCCESS;
}

BOOL MakeCS_PlasmaFlop( CS_FRAME *f, CS_FRAME *c, CS_BITMAPINFO *p, COLORREF *pals )
{
	int pX = _x( f );
	int pY = _y( f );

	int x, y;

	float fX = (float) ( _rr( c ) - _rl( c ) + 1 ) / pX;
	float fY = (float) ( _rb( c ) - _rt( c ) + 1 ) / pY;

	if ( ( fX <= 0 ) || ( fY <= 0 ) ) return ERROR_CURSORSHOP( 0x27 );

	for ( y = pY - 1; y >= 0; y -- )
	for ( x = pX - 1; x >= 0; x -- )
	{
		int oX = (int) ( x * fX ) + _rl( c );
		int oY = (int) ( y * fY ) + _rt( c );

		COLORREF col = _PX( c, oX, oY );
		if ( !col ) continue;

		_PX( f, x, y ) = pals[ _PX( p, x, y ) ] | CS_ALPHA_MASK;
	}

	_cx( f ) = ( _cx( c ) - _rl( c ) ) / fX;
	_cy( f ) = ( _cy( c ) - _rt( c ) ) / fY;

	return ERROR_SUCCESS;
}

BOOL MakeCS_PlasmaInfo( CS_PLASMAINFO *f, CS_HEADER *h, CS_FRAME *c )
{
	BOOL iMake;
	int pX, pY;

	pX = h->destSize.cx * h->aaFactor;
	pY = h->destSize.cy * h->aaFactor;

	iMake = MakeCS_BitmapContext( &f->plas, pX, pY );
	if ( iMake ) return ERROR_CURSORSHOP( 0x20 );

	srand( h->randSeed );

	iMake = PlasmaCS_BitmapInfo( &f->plas.bmi, (int) ( h->plasmaChaos * 1000 ), 0 );
	if ( iMake ) return ERROR_CURSORSHOP( 0x21 );

	iMake = PlasmaCS_ColorInfo( f->cols, &c->bcx.bmi, &f->cNum );
	if ( iMake ) return ERROR_CURSORSHOP( 0x24 );

	iMake = PlasmaCS_ColorRef( f->pals, f->cols, f->cNum );
	if( iMake ) return ERROR_CURSORSHOP( 0x26 );

	iMake = MakeCS_Frame( &f->flop, pX, pY );
	if ( iMake ) return ERROR_CURSORSHOP( 0x28 );

	iMake = MakeCS_PlasmaFlop( &f->flop, c, &f->plas.bmi, f->pals );
	if ( iMake ) return ERROR_CURSORSHOP( 0x29 );

	return ERROR_SUCCESS;
}

BOOL KillCS_PlasmaInfo( CS_PLASMAINFO *f )
{
	BOOL iKill = KillCS_BitmapContext( &f->plas ) | KillCS_Frame( &f->flop );

	return iKill ? ERROR_CURSORSHOP( 0x25 ) : ERROR_SUCCESS;
}
