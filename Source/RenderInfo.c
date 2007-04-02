#include "CursorShop.h"

#include <math.h>

CS_FRAME *RoteCS_Frame( CS_FRAME *o, CS_HEADER *h, float fi )
{
    CS_FRAME *f = 0;

    DWORD oX = o->_x;
	DWORD oY = o->_y;

	float stepHX, stepHY;
	float stepVX, stepVY;

    float x1, y1, x2, y2;

    float minX, maxX;
    float minY, maxY;

    float scaleV, scaleH;
    float aspectX, aspectY;

    BYTE aaF;

    DWORD wX, wY;
    DWORD sS, sD;

    int x, y;

    __asm
    {
        finit
        fld     DWORD PTR [ fi ]
        fsincos
        fstp    DWORD PTR [ stepHX ]
        fstp    DWORD PTR [ stepHY ]
        fwait
    }

    stepVX = - stepHY / CS_ASPECT_CORRECTION;
	stepVY =   stepHX / CS_ASPECT_CORRECTION;

	x1 = oX * stepHX;
	y1 = oX * stepHY;
	x2 = oY * stepVX;
	y2 = oY * stepVY;

	minX = min( min( 0, x1 ), min( x2, x1 + x2 ) );
	minY = min( min( 0, y1 ), min( y2, y1 + y2 ) );
	maxX = max( max( 0, x1 ), max( x2, x1 + x2 ) );
	maxY = max( max( 0, y1 ), max( y2, y1 + y2 ) );

	scaleV = stepVX / stepVY;
	scaleH = stepHY / stepHX;

	aspectX = oX / ( oX * ( stepHX - stepHY * scaleV ) );
	aspectY = oY / ( oY * ( stepVY - stepVX * scaleH ) );

	wX = (DWORD) ( maxX - minX + 1 );
	wY = (DWORD) ( maxY - minY + 1 );

    aaF = h->aaFactor;
	sS = (DWORD) ( h->shadowDist * aaF + 0.5f );
	sD = ( 1 + wX + sS ) * sS;

    f = MakeCS_Frame( wX + sS, wY + sS );
    if ( !f ) return 0;

	for ( y = wY - 1; y >= 0; y -- )
	for ( x = wX - 1; x >= 0; x -- )
	{
		float fX = ( ( x + minX ) - ( y + minY ) * scaleV ) * aspectX;
		float fY = ( ( y + minY ) - ( x + minX ) * scaleH ) * aspectY;

		COLORREF col, *cur;

		if ( ( fX < 0 ) || ( fX >= oX ) ) continue;
		if ( ( fY < 0 ) || ( fY >= oY ) ) continue;

		col = _PX( o, (DWORD) fX, (DWORD) fY );
		if ( ! col ) continue;

		cur = &_PX( f, x, y );

		if ( ! cur[ sD ] )
		{
			cur[ sD ] = ( col == CS_SHADOW_OPAQ ) ? CS_SHADOW_LITE : CS_SHADOW_DARK;

            TestCS_Frame( f, x + sS, y + sS );
		}

		*cur = col;

		TestCS_Frame( f, x, y );
	}


    f->_fx = o->_fx * x1 / oX + o->_fy * x2 / oY - minX;
    f->_fy = o->_fx * y1 / oX + o->_fy * y2 / oY - minY;

    f->_rl -= ( ( aaF - (int)(DWORD)( f->_fx - f->_rl ) ) % aaF + aaF ) % aaF;
    f->_rt -= ( ( aaF - (int)(DWORD)( f->_fy - f->_rt ) ) % aaF + aaF ) % aaF;

    f->_rr = ( ( f->_rr - f->_rl ) / aaF + 1 ) * aaF + f->_rl - 1;
    f->_rb = ( ( f->_rb - f->_rt ) / aaF + 1 ) * aaF + f->_rt - 1;

    return f;
}

CS_FRAME *SrnkCS_Frame( CS_FRAME *o, BYTE aaF )
{
	int lX = o->_x - 1;
	int lY = o->_y - 1;

	int wX = ( o->_rr - o->_rl + 1 ) / aaF;
	int wY = ( o->_rb - o->_rt + 1 ) / aaF;

	int x, y;

    DWORD aaS = aaF * aaF;

    CS_FRAME *f = MakeCS_Frame( wX, wY );
	if ( !f ) return 0;

	for ( y = wY - 1; y >= 0; y -- )
	for ( x = wX - 1; x >= 0; x -- )
	{
		DWORD argb[ 4 ] = { 0 };
		int a, b, c;

		int sX = x * aaF + o->_rl;
		int sY = y * aaF + o->_rt;

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
				bPx[ c ] = (BYTE) ( argb[ c ] / aaS );
			}
		}
	}

	f->_ix = (int)(DWORD)( ( o->_fx - o->_rl ) / aaF );
	f->_iy = (int)(DWORD)( ( o->_fy - o->_rt ) / aaF );

	return f;
}

CS_FRAME *RendCS_Frame( CS_FACTORY *o, float fi )
{
    TCHAR buffy[ MAX_PATH ];
    HDC dDC;

	BYTE iMake = 0x60;
	CS_FRAME *n = 0, *f;

    LARGE_INTEGER freq, t0, t1, t2;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&t0);

    f = RoteCS_Frame( o->rndy, &o->head, fi );
    if ( !f ) goto MakeError; iMake ++;

    QueryPerformanceCounter(&t1);

    n = SrnkCS_Frame( f, o->head.aaFactor );
    if ( !n ) goto MakeError; iMake ++;

    QueryPerformanceCounter(&t2);

    KillCS_Frame( f );

    dDC = GetDC( 0 );
    wsprintf( buffy, TEXT( "P1: %d    " ), ((t1.QuadPart-t0.QuadPart)*1000000)/freq.QuadPart );
    TextOut( dDC, 50, 100, buffy, lstrlen( buffy ) );
    wsprintf( buffy, TEXT( "P2: %d    " ), ((t2.QuadPart-t1.QuadPart)*1000000)/freq.QuadPart );
    TextOut( dDC, 50, 150, buffy, lstrlen( buffy ) );
    ReleaseDC( 0, dDC );

    return n;

MakeError:

    MessageBox( 0, TEXT( "RendCS_Frame crash!" ), 0, MB_ICONHAND );

    KillCS_Frame( n );
    KillCS_Frame( f );

    SetLastError( ERROR_CURSORSHOP( iMake ) );
    return 0;
}
