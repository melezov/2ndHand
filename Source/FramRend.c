#include "FramRend.h"

IS_FRAME *RoteIS_FRAME( IS_FRAME *o, DWORD aaF, DWORD sS, float stepHX, float stepHY  )
{
    DWORD dErr = ERROR_FRAMREND( 0x00 );

    const DWORD oX = o->_x;
    const DWORD oY = o->_y;

    const float stepVX = - stepHY / IS_ASPECT_CORRECTION;
    const float stepVY =   stepHX / IS_ASPECT_CORRECTION;

    const float scaleV = stepVX / stepVY;
    const float scaleH = stepHY / stepHX;

    const float aspectX = oX / ( oX * ( stepHX - stepHY * scaleV ) );
    const float aspectY = oY / ( oY * ( stepVY - stepVX * scaleH ) );

    const float splitQ = scaleV * aspectX;
    const float splitW = scaleH * aspectY;

    const float x1 = oX * stepHX;
    const float y1 = oX * stepHY;
    const float x2 = oY * stepVX;
    const float y2 = oY * stepVY;

    const float minX = min( min( 0, x1 ), min( x2, x1 + x2 ) );
    const float minY = min( min( 0, y1 ), min( y2, y1 + y2 ) );
    const float maxX = max( max( 0, x1 ), max( x2, x1 + x2 ) );
    const float maxY = max( max( 0, y1 ), max( y2, y1 + y2 ) );

    DWORD wX = (DWORD) ( maxX - minX + 1 );
    DWORD wY = (DWORD) ( maxY - minY + 1 );

    const DWORD sX = wX + aaF + sS;
    const DWORD sY = wY + aaF + sS;

    const float mX = o->_fx * x1 / oX + o->_fy * x2 / oY - minX;
    const float mY = o->_fx * y1 / oX + o->_fy * y2 / oY - minY;

    const int dX = aaF - (int)(DWORD) mX % aaF;
    const int dY = aaF - (int)(DWORD) mY % aaF;

    const DWORD sD = ( 1 + sX + dX ) * sS;

    IS_FRAME *f;

    int x, y, *lutXQ = 0, *lutXW, *lutYQ, *lutYW;

    CHECK_ERROR( f = MakeIS_FRAME( sX + dX, sY + dY ) );
    CHECK_ERROR( lutXQ = (int *) MemAlloc( ( wX + wY ) * 2 ) );

    lutXW = lutXQ + wX;
    lutYQ = lutXW + wX;
    lutYW = lutYQ + wY;

    for ( x = -- wX; x >= 0; x -- )
    {
        lutXQ[ x ] = (int)(DWORD) ( ( x + minX ) * aspectX * 256 );
        lutXW[ x ] = (int)(DWORD) ( ( x + minX ) * splitW  * 256 );
    }

    for ( y = -- wY; y >= 0; y -- )
    {
        lutYQ[ y ] = (int)(DWORD) ( ( y + minY ) * aspectY * 256 );
        lutYW[ y ] = (int)(DWORD) ( ( y + minY ) * splitQ  * 256 );
    }

    for ( y = wY; y >= 0; y -- )
    {
        DWORD *dPx = &_PX( f, dX + wX, y + dY );

        const int curYW = lutYW[ y ];
        const int curYQ = lutYQ[ y ];

        for ( x = wX; x >= 0; x --, dPx -- )
        {
            const int fX = (int) ( lutXQ[ x ] - curYW ) >> 8;
            const int fY = (int) ( curYQ - lutXW[ x ] ) >> 8;

	        COLORREF col;

            if ( ( fX < 0 ) || ( fX >= (int) oX ) ) continue;
	        if ( ( fY < 0 ) || ( fY >= (int) oY ) ) continue;

	        if ( !( col = _PX( o, (DWORD) fX, (DWORD) fY ) ) ) continue;

            *dPx = col;

            if ( ! dPx[ sD ] )
	        {
                dPx[ sD ] = ( col == IS_SHADOW_OPAQ ) ? IS_SHADOW_LITE : IS_SHADOW_DARK;
            }

            TestIS_FRAME( f, x + dX, y + dY );
        }
    }

    f->_rr += sS;
    f->_rb += sS;

    f->_ix = ( ( (int)(DWORD) mX ) / aaF + 1 ) * aaF;
    f->_iy = ( ( (int)(DWORD) mY ) / aaF + 1 ) * aaF;

    dErr = ERROR_SUCCESS;

CatchError:

    MemFree( lutXQ );

    if ( dErr )
    {
        KillIS_FRAME( f );
        f = 0;

        MsgErr( dErr, MB_ICONINFORMATION );
    }

    return f;
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

    float stepHX, stepHY;

    const DWORD aaF = f->head.aaFactor;
    const DWORD sS = MulHalfAdd( aaF, f->head.shadowDist );

    __asm
    {
        finit
        fld     DWORD PTR [ fi ]
        fsincos
        fstp    DWORD PTR [ stepHX ]
        fstp    DWORD PTR [ stepHY ]
        fwait
    }

    CHECK_ERROR( o = RoteIS_FRAME( f->rndy, aaF, sS, stepHX, stepHY ) );
    CHECK_ERROR( n = SrnkIS_FRAME( o, f->head.aaFactor ) );

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
