#include "SharedCode.h"
#include "CursorShop.h"

typedef struct
{
    int x0, y0;
    int x1, y1;
    int xC, yC;

    CS_FRAME *f;

    DWORD chaos, seed;
    DWORD maxPx;
}
CS_PLASMA;

static const DWORD plcnt[] =
{
      0,  4,  8,  4, 16,  4,  0,  4, 16, 20,
      8,  4,  8, 12,  8, 20, 16,  4,  8, 20,
      0, 12,  8, 12, 16, 12, 16, 20,  8, 12,
      0,  4,  0, 12,  0, 20,  0, 20, 16, 12
};

#define DWORD_REF( x, y ) ( * (DWORD *) ( (DWORD) ( x ) + ( y ) ) )

void PlasmaDivide( CS_PLASMA *p )
{
    CS_PLASMA n;
    DWORD i, *tPx;
    DWORD w, *pPx;

    MemCopy( sizeof( CS_PLASMA ) / 4, &n, p );

    pPx = n.f->dPx;
    w = n.f->_x;

    tPx = &pPx[ n.xC + n.yC * w ];

    if ( ! *tPx )
    {
        DWORD col = 0;

        for ( i = 0; i < 4; i ++ )
        {
            const DWORD *pls = plcnt + i * 10;
            DWORD *dPx = &pPx[ DWORD_REF( &n, pls[ 4 ] ) + DWORD_REF( &n, pls[ 5 ] ) * w ];

            DWORD cur = *dPx;

            if ( ! cur )
            {
                int r = n.chaos;

                if ( r )
                {
                    r = NRandom( &p->seed ) % r;
                    if ( NRandom( &p->seed ) & 1 ) r = - r;
                }

                r = ( ( pPx[ DWORD_REF( &n, pls[ 0 ] ) + DWORD_REF( &n, pls[ 1 ] ) * w ] +
                        pPx[ DWORD_REF( &n, pls[ 2 ] ) + DWORD_REF( &n, pls[ 3 ] ) * w ] + 1 ) >> 1 ) + r;

                if ( r < 1 ) r = 1; else if ( r > (int) n.maxPx ) r = n.maxPx;

                cur = *dPx = r;
            }

            col += cur;
        }

        *tPx = ( col + 2 ) >> 2;
    }

    n.seed = p->seed;
    n.chaos >>= 1;

    for ( i = 0; i < 4; i ++ )
    {
        DWORD r;
        const DWORD *pls = &plcnt[ i * 10 + 6 ];

        for ( r = 0; r < 4; r ++ )
        {
            ( (DWORD *) &n )[ r ] = DWORD_REF( p, pls[ r ] );
        }

        if ( ( n.x1 - n.x0 > 1 ) || ( n.y1 - n.y0 > 1 ) )
        {
            n.xC = ( n.x0 + n.x1 ) >> 1;
            n.yC = ( n.y0 + n.y1 ) >> 1;

            PlasmaDivide( &n );
        }
   }
}

void PlasmaCS_Frame( CS_FRAME *f, CS_HEADER *h )
{
    CS_PLASMA iP;

    iP.x0 = 0;
    iP.y0 = 0;

    iP.xC = ( iP.x1 = f->_x - 1 ) >> 1;
    iP.yC = ( iP.y1 = f->_y - 1 ) >> 1;

    iP.f = f;
    iP.maxPx = 255;

    iP.chaos = (DWORD) ( h->plasmaChaos * iP.maxPx );
    iP.seed = h->randSeed;

    _PX( f->,     0,     0 ) = 1 + NRandom( &iP.seed ) % ( iP.maxPx - 1 );
    _PX( f->, iP.x1,     0 ) = 1 + NRandom( &iP.seed ) % ( iP.maxPx - 1 );
    _PX( f->,     0, iP.y1 ) = 1 + NRandom( &iP.seed ) % ( iP.maxPx - 1 );
    _PX( f->, iP.x1, iP.y1 ) = 1 + NRandom( &iP.seed ) % ( iP.maxPx - 1 );

    PlasmaDivide( &iP );
}
