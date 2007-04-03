#include "CursorShop.h"

#define CS_PLASMA_MAX_COLORS 0xFF

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

typedef struct
{
    int x0, y0;
    int x1, y1;
    int xC, yC;
}
CS_PLASMA;

void PlasmaDivide( CS_PLASMA *p, CS_FRAME *f, DWORD *seed, DWORD chaos )
{
    CS_PLASMA kr[ 8 ];

    DWORD *iPx = &_PX( f, p->xC, p->yC );

    DWORD cycleBgn = *iPx ? 4 : 0;
	DWORD cycleEnd = ( p->x1 - p->x0 > 2 ) || ( p->y1 - p->y0 > 2 ) ? 8 : 4;

    int i = 0;

    for ( ; cycleBgn < cycleEnd; cycleBgn ++ )
	{
        CS_PLASMA *c = &kr[ cycleBgn ];

        const BYTE *pI = &plasmaIndexes[ cycleBgn * 6 ];
        DWORD *kI = (DWORD *) c;

        int k;

        for ( k = 5; k >= 0; k -- )
        {
            *kI = ( (DWORD *) p )[ *pI ];

            kI ++;
            pI ++;
        }

		if ( cycleBgn < 4 )
		{
            DWORD *vPx = &_PX( f, c->xC, c->yC );
            int col = *vPx;

			if ( ! col )
			{
                col = ( _PX( f, c->x0, c->y0 ) + _PX( f, c->x1, c->y1 ) + 1 ) >> 1;

                if ( chaos )
                {
                    int rand = NRandom( seed ) % chaos;
                    if ( NRandom( seed ) & 1 ) rand = -rand;

                    col += rand;
                }

				if ( col < 1 ) col = 1; else if ( col > CS_PLASMA_MAX_COLORS ) col = CS_PLASMA_MAX_COLORS;

			   *vPx = col;
			}

			i += col;

            if ( cycleBgn == 3 )
            {
                *iPx = ( i + 2 ) >> 2;

                chaos >>= 1;
            }
		}

        else if ( ( c->x1 - c->x0 > 1 ) || ( c->y1 - c->y0 > 1 ) )
		{
            c->xC = ( c->x0 + c->x1 + 1 ) >> 1;
            c->yC = ( c->y0 + c->y1 + 1 ) >> 1;

			PlasmaDivide( c, f, seed, chaos );
		}
	}
}

void PlasmaInit( CS_FRAME *f, CS_HEADER *h )
{
    CS_PLASMA iP;

    DWORD seed = h->randSeed;
    DWORD chaos = (DWORD) ( h->plasmaChaos * CS_PLASMA_MAX_COLORS );

    iP.x0 = 0;
    iP.y0 = 0;

    iP.xC = ( iP.x1 = f->_x - 1 ) >> 1;
    iP.yC = ( iP.y1 = f->_y - 1 ) >> 1;

    _PX( f,     0,     0 ) = 1 + NRandom( &seed ) % ( CS_PLASMA_MAX_COLORS - 1 );
    _PX( f, iP.x1,     0 ) = 1 + NRandom( &seed ) % ( CS_PLASMA_MAX_COLORS - 1 );
    _PX( f,     0, iP.y1 ) = 1 + NRandom( &seed ) % ( CS_PLASMA_MAX_COLORS - 1 );
    _PX( f, iP.x1, iP.y1 ) = 1 + NRandom( &seed ) % ( CS_PLASMA_MAX_COLORS - 1 );

    PlasmaDivide( &iP, f, &seed, chaos );
}

BOOL PlasmaPalette( CS_FRAME *f, DWORD *pals )
{
    DWORD cols[ 2 ];

    DWORD cCnt = 0;
	DWORD wCnt = 0;

    int x, y;

	for ( y = f->_y - 1; y >= 0; y -- )
	for ( x = f->_x - 1; x >= 0; x -- )
	{
		DWORD *col = &_PX( f, x, y );
        DWORD cur = *col;

		if ( ! cur ) continue;

		if ( cur == CS_WHITE )
		{
			f->_fx += x;
			f->_fy += y;
			wCnt ++;

			*col = 0;
		}
		else
        {
            TestCS_Frame( f, x, y );

            if ( !cCnt || ( cCnt == 1 ) && ( cols[ 0 ] != cur ) )
            {
                cols[ cCnt ++ ] = cur;
            }
        }
	}

    if ( !wCnt || !cCnt ) return 0;

    f->_fx = f->_fx / wCnt + 0.5f;
	f->_fy = f->_fy / wCnt + 0.5f;

    if ( ( pals[ 0 ] = cCnt ) == 1 )
    {
        pals[ 1 ] = cols[ 0 ];
        return 1;
    }

    x = CS_PLASMA_MAX_COLORS - 1;
    y = 0;

    while ( x >= 0 )
    {
        BYTE *src = (BYTE *) cols + 2;
        BYTE *dst = (BYTE *) ( pals + x + 1 ) + 2;

        for ( ; src >= (BYTE *) cols; src --, dst -- )
        {
            *dst = (BYTE) ( ( src[ 0 ] * x + src[ 4 ] * y ) / ( CS_PLASMA_MAX_COLORS - 1 ) );
        }

        x --;
        y ++;
    }

    return cCnt;

}
extern CS_FRAME *last;

CS_FRAME *BlotCS_Frame( DWORD rad )
{
    int x, y;

    DWORD diam = ( rad << 1 ) + 1;
    DWORD radS = ( rad + 1 ) * ( rad + 1 );

    CS_FRAME *f = MakeCS_Frame( diam, diam );
    if ( !f ) return 0;

    for ( y = diam - 1; y >= 0; y -- )
    for ( x = diam - 1; x >= 0; x -- )
    {
        DWORD dX = abs( x - rad );
        DWORD dY = abs( y - rad );

        if ( dX * dX + dY * dY < radS ) _PX( f, x, y ) = CS_SHADOW_OPAQ;
    }

    return f;
}

CS_FRAME *PlsmCS_Frame( CS_FRAME *o, CS_HEADER *h )
{
    BYTE iMake = 0x40;

    DWORD pals[ CS_PLASMA_MAX_COLORS + 1 ];
    DWORD cols;

    BYTE aaF = h->aaFactor;

    int pX = h->destSize.cx * aaF;
    int pY = h->destSize.cy * aaF;

    int x, y, z;
    float fX, fY;

    DWORD bS = (DWORD) ( h->borderWidth * aaF + 0.5f );

    CS_FRAME *f = 0, *n = 0, *b = 0;

    cols = PlasmaPalette( o, pals );
    if ( !cols ) goto MakeError; iMake ++;

    fX = (float) ( o->_rr - o->_rl + 1 ) / pX;
    fY = (float) ( o->_rb - o->_rt + 1 ) / pY;

    if ( ( fX <= 0 ) || ( fY <= 0 ) ) goto MakeError; iMake ++;

    f = MakeCS_Frame( pX, pY );
    if ( !f ) goto MakeError; iMake ++;

    PlasmaInit( f, h );

    for ( y = pY - 1; y >= 0; y -- )
    for ( x = pX - 1; x >= 0; x -- )
    {
        int oX = o->_rl + (DWORD) ( x * fX );
	    int oY = o->_rt + (DWORD) ( y * fY );

        DWORD *cur = &_PX( f, x, y );
        DWORD col = _PX( o, oX, oY );

        if ( col ) col = pals[ *cur ] | CS_ALPHA_MASK;

        *cur = col;
    }

    f->_fx = ( o->_fx - o->_rl ) / fX + bS;
    f->_fy = ( o->_fy - o->_rt ) / fY + bS;

    if ( !bS ) return f;

    n = MakeCS_Frame( pX + ( bS << 1 ), pY + ( bS << 1 ) );
    if ( !n ) goto MakeError; iMake ++;

    b = BlotCS_Frame( bS );
    if ( !b ) goto MakeError; iMake ++;

    for ( z = 0; z < 2; z ++ )
    {
        for ( y = pY - 1; y >= 0; y -- )
        for ( x = pX - 1; x >= 0; x -- )
        {
            DWORD *cur = &_PX( f, x, y );
            DWORD col = *cur;

            if ( col )
            {
                if ( !z )
                {
                    if ( ( x && cur[ -  1 ] ) && ( ( x < pX - 1 ) && cur[  1 ] ) )
                    if ( ( y && cur[ - pX ] ) && ( ( y < pY - 1 ) && cur[ pX ] ) ) continue;

                    BitBlt( n->hDC, x, y, b->_x, b->_y, b->hDC, 0, 0, SRCPAINT );
                }
                else _PX( n, x + bS, y + bS ) = col;
            }
        }
    }

    n->_fx = f->_fx;
    n->_fy = f->_fy;

    KillCS_Frame( b );
    KillCS_Frame( f );

    return n;

MakeError:

    MessageBox( gm_hWnd, TEXT( "PlsmCS_Frame crash!" ), 0, MB_ICONHAND );

    KillCS_Frame( b );
    KillCS_Frame( n );
    KillCS_Frame( f );

    SetLastError( ERROR_CURSORSHOP( iMake ) );
    return 0;
}
