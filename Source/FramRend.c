#include "FramRend.h"

typedef struct
{
    DWORD width;
    int *lutQ, *lutW;
    int mid;
    DWORD delta;
}
FR_INFO;

FR_INFO *MakeFR_INFO( DWORD aaF, int oX, int oY, float fi, float fx, float fy, float sign );

IS_FRAME *RoteIS_FRAME( IS_FRAME *o, DWORD aaF, DWORD sS, float fi )
{
    DWORD dErr = ERROR_FRAMREND( 0x00 );

    FR_INFO *rX, *rY = 0;

    CHECK_ERROR( rX = MakeFR_INFO( aaF, o->_x, - (int) o->_y, fi, o->_fx, - o->_fy, 1 ) );
    CHECK_ERROR( rY = MakeFR_INFO( aaF, o->_y, o->_x, fi, o->_fy, o->_fx, -1 ) );

    {
        const DWORD oX = o->_x;
        const DWORD oY = o->_y;

        DWORD wX = rX->width;
        DWORD wY = rY->width;

        const int dX = rX->delta;
        const int dY = rY->delta;

        const DWORD zX = wX + aaF + sS + dX;
        const DWORD zY = wY + aaF + sS + dY;

        const DWORD sD = ( 1 + zX ) * sS;

        IS_FRAME *f = 0;

        int x, y;

        int rl = wX;
        int rt = 0;
        int rr = 0;
        int rb = -1;

        CHECK_ERROR( f = MakeIS_FRAME( zX, zY ) );

        for ( y = wY - 1; y >= 0; y -- )
        {
            const int curYQ = rY->lutQ[ y ];
            const int curYW = rY->lutW[ y ];

            int sX = -1, eX;

            DWORD *dPx = &_PX( f, dX + sX, y + dY ), col;

            for ( x = 0; x < (int) wX; x ++, dPx ++ )
            {
                const int fX = (int) ( rX->lutQ[ x ] - curYW ) >> 8;
                const int fY = (int) ( curYQ - rX->lutW[ x ] ) >> 8;

                if ( ( fX < 0 ) || ( fY < 0 ) || ( fX >= (int) oX ) || ( fY >= (int) oY ) )
                {
                    if ( sX != -1 ) break;
                }
                else if ( *dPx = col = _PX( o, (DWORD) fX, (DWORD) fY ) )
                {
                    if ( sX == -1 ) sX = x;
                    eX = x;

                    if ( col && !dPx[ sD ] )
	                {
                        dPx[ sD ] = ( col == IS_SHADOW_OPAQ ) ? IS_SHADOW_LITE : IS_SHADOW_DARK;
                    }
                }
            }

            if ( sX != -1 )
            {
                if ( rl > sX ) rl = sX;
                if ( rr < eX ) rr = eX;

                rt = y;
                if ( rb == -1 ) rb = y;
            }
        }

        TestIS_FRAME( f, rl + dX, rt + dY );
        TestIS_FRAME( f, rr + dX + sS, rb + dY + sS );

        f->_ix = rX->mid;
        f->_iy = rY->mid;

        dErr = ERROR_SUCCESS;

    CatchError:

        MemFree( rX );
        MemFree( rY );

        if ( dErr )
        {
            KillIS_FRAME( f );
            f = 0;

            MsgErr( dErr, MB_ICONINFORMATION );
        }

        return f;
    }
}

void ShrinkRasterMMX( DWORD aaF, DWORD tX, DWORD *dPx, DWORD wXm, DWORD wYm, DWORD *tPx );

IS_FRAME *SrnkIS_FRAME( IS_FRAME *o, DWORD aaF )
{
    DWORD dErr = ERROR_FRAMREND( 0x10 );

    const DWORD sX = ( o->_rl / aaF );
    const DWORD sY = ( o->_rt / aaF );

    const DWORD eX = ( o->_rr / aaF );
    const DWORD eY = ( o->_rb / aaF );

    const DWORD wX = eX - sX;
    const DWORD wY = eY - sY;

    IS_FRAME *f = 0;

    CHECK_ERROR( ( aaF > 1 ) && ( aaF <= IS_AAF_MAX ) );
    CHECK_ERROR( f = MakeIS_FRAME( wX + 1, wY + 1 ) );

    ShrinkRasterMMX( aaF, o->_x, &_PX( o, eX * aaF, eY * aaF ), wX, wY, &_PX( f, wX, wY ) );

    f->_ix = o->_ix / (int) aaF - sX;
    f->_iy = o->_iy / (int) aaF - sY;

    dErr = ERROR_SUCCESS;

CatchError:

    if ( dErr )
    {
        KillIS_FRAME( f );
        f = 0;

        MsgErr( dErr, MB_ICONINFORMATION );
    }

    return f;
}

IS_FRAME *RendIS_FRAME( IS_FACTORY *f, float fi )
{
    DWORD dErr = ERROR_FRAMREND( 0x20 );
	IS_FRAME *n = 0, *o;

    const DWORD aaF = f->head.aaFactor;
    const DWORD sS = MulHalfAdd( aaF, f->head.shadowDist );

    CHECK_ERROR( o = RoteIS_FRAME( f->rndy, aaF, sS, fi ) );

    if ( aaF == 1 ) return o;

    CHECK_ERROR( n = SrnkIS_FRAME( o, aaF ) );

    dErr = ERROR_SUCCESS;

CatchError:

    KillIS_FRAME( o );

    if ( dErr )
    {
        KillIS_FRAME( n );
        n = 0;

        MsgErr( dErr, MB_ICONWARNING );
    }

    return n;
}
