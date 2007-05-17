#include "PlasRend.h"
#include "IconShop.h"

DWORD AdjustIS_PLASMA( IS_FRAME *f, DWORD *seed, DWORD grad, DWORD xa, DWORD ya, DWORD x0, DWORD y0, DWORD xb, DWORD yb )
{
    DWORD *cur = &_PX( f, x0, y0 );
    DWORD col = *cur;

    int rand;
    if ( col ) return col; // HERE BE DRAGONS

    rand = (int) NRandom( seed, grad );
    if ( !( rand & 1 ) ) rand = - rand;

    rand = ( (int) ( _PX( f, xa, ya ) + _PX( f, xb, yb ) ) + rand ) >> 1;
    if ( rand < 1 ) rand = 1; else if ( rand > IS_PLASMA_MAX_COLORS ) rand = IS_PLASMA_MAX_COLORS;

   return *cur = rand;
}

void DivideIS_PLASMA( IS_FRAME *f, DWORD *seed, DWORD grad, DWORD x1, DWORD y1, DWORD x2, DWORD y2 )
{
    DWORD x0, y0, *cur;
    DWORD nseed;

    if ( ( x2 - x1 < 2 ) && ( y2 - y1 < 2 ) ) return;

    x0 = ( x1 + x2 + 1 ) >> 1;
    y0 = ( y1 + y2 + 1 ) >> 1;

    if ( ! *( cur = &_PX( f, x0, y0 ) ) )
    {
        *cur = ( AdjustIS_PLASMA( f, seed, grad, x1, y1, x0, y1, x2, y1 ) +
                 AdjustIS_PLASMA( f, seed, grad, x2, y1, x2, y0, x2, y2 ) +
                 AdjustIS_PLASMA( f, seed, grad, x1, y2, x0, y2, x2, y2 ) +
                 AdjustIS_PLASMA( f, seed, grad, x1, y1, x1, y0, x1, y2 ) + 2 ) >> 2;
    }

    nseed = *seed + *cur;

    grad >>= 1;

    DivideIS_PLASMA( f, &nseed, grad, x1, y1, x0, y0 );
    DivideIS_PLASMA( f, &nseed, grad, x0, y1, x2, y0 );
    DivideIS_PLASMA( f, &nseed, grad, x0, y0, x2, y2 );
    DivideIS_PLASMA( f, &nseed, grad, x1, y0, x0, y2 );
}

void InitIS_PLASMA( IS_FRAME *f, DWORD seed, float plasmaChaos )
{
    DWORD chaos = (DWORD) ( plasmaChaos * IS_PLASMA_MAX_COLORS );

    DWORD x = f->_x - 1;
    DWORD y = f->_y - 1;

    _PX( f, 0, 0 ) = 1 + NRandom( &seed, IS_PLASMA_MAX_COLORS - 1 );
    _PX( f, x, 0 ) = 1 + NRandom( &seed, IS_PLASMA_MAX_COLORS - 1 );
    _PX( f, 0, y ) = 1 + NRandom( &seed, IS_PLASMA_MAX_COLORS - 1 );
    _PX( f, x, y ) = 1 + NRandom( &seed, IS_PLASMA_MAX_COLORS - 1 );

    DivideIS_PLASMA( f, &seed, chaos, 0, 0, x, y );
}

BOOL EnumIS_PLASMA( IS_FRAME *f, DWORD *pals )
{
    DWORD cols[ 2 ];

    DWORD cCnt = 0;
    DWORD wCnt = 0;

    int x, y;

    for ( y = f->_y - 1; y >= 0; y -- )
    for ( x = f->_x - 1; x >= 0; x -- )
    {
        DWORD *col = &_PX( f, x, y );
        DWORD cur = *col & IS_WHITE;

        if ( ! cur ) continue;

        if ( cur == IS_WHITE )
        {
            f->_fx += x;
            f->_fy += y;
            wCnt ++;

            *col = 0;
        }
        else
        {
            TestIS_FRAME( f, x, y );

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

    x = IS_PLASMA_MAX_COLORS - 1;

    for ( y = x; x >= 0; x -- )
    {
        BYTE *src = (BYTE *) cols + 2;
        BYTE *dst = (BYTE *) ( pals + x + 1 ) + 2;

        for ( ; src >= (BYTE *) cols; src --, dst -- )
        {
            *dst = (BYTE) ( ( src[ 0 ] * x + src[ 4 ] * ( y - x ) ) / y );
        }
    }

    return cCnt;
}

IS_FRAME *MakeIS_PLASMA( IS_FRAME *o, IS_HEADER *h )
{
    DWORD dErr = ERROR_PLASREND( 0x00 );

    DWORD pals[ IS_PLASMA_MAX_COLORS + 1 ];
    DWORD aaF = h->aaFactor;

    int pX = h->destSize.cx * aaF;
    int pY = h->destSize.cy * aaF;

    int x, y, z;
    float fX, fY;

    DWORD bS = MulHalfAdd( aaF, h->borderWidth );

    IS_FRAME *f = 0, *n = 0, *b = 0;
    CHECK_ERROR( EnumIS_PLASMA( o, pals ) );

    fX = (float) ( o->_rr - o->_rl + 1 ) / pX;
    fY = (float) ( o->_rb - o->_rt + 1 ) / pY;

    CHECK_ERROR( ( fX > 0 ) && ( fY > 0 ) );
    CHECK_ERROR( f = MakeIS_FRAME( pX, pY ) );

    InitIS_PLASMA( f, h->randSeed, h->plasmaChaos );

    for ( y = pY - 1; y >= 0; y -- )
    for ( x = pX - 1; x >= 0; x -- )
    {
        int oX = o->_rl + (DWORD) ( x * fX );
        int oY = o->_rt + (DWORD) ( y * fY );

        DWORD *cur = &_PX( f, x, y );
        DWORD col = _PX( o, oX, oY );

        if ( col ) col = pals[ *cur ] | IS_ALPHA_MASK;

        *cur = col;
    }

    f->_fx = ( o->_fx - o->_rl ) / fX + bS;
    f->_fy = ( o->_fy - o->_rt ) / fY + bS;

    if ( !bS ) return f;

    CHECK_ERROR( n = MakeIS_FRAME( pX + ( bS << 1 ), pY + ( bS << 1 ) ) );
    CHECK_ERROR( b = BlotIS_PLASMA( bS, IS_SHADOW_OPAQ ) );

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

    dErr = ERROR_SUCCESS;

CatchError:

    KillIS_FRAME( b );
    KillIS_FRAME( f );

    if ( dErr )
    {
        KillIS_FRAME( n );
        n = 0;

        MsgErr( dErr, MB_ICONWARNING );
    }

    return n;
}
