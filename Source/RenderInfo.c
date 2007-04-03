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
    float splitQ, splitW;

    BOOL firstX, firstY;
    BOOL lastX, lastY, finalX;

    BYTE aaF;

    DWORD wX, wY;
    DWORD sS, sD;

    DWORD *dPx;

    int *lutXQ, *lutYQ, *lutXW, *lutYW;

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

    splitQ = scaleV * aspectX;
    splitW = scaleH * aspectY;

	wX = (DWORD) ( maxX - minX + 1 );
	wY = (DWORD) ( maxY - minY + 1 );

    aaF = h->aaFactor;
	sS = (DWORD) ( h->shadowDist * aaF + 0.5f );
	sD = ( 1 + wX + sS ) * sS;

    lutXQ = (int *) MemAlloc( ( wX + wY ) << 1 );
    if ( !lutXQ ) return 0;

    lutXW = lutXQ + wX;
    lutYQ = lutXW + wX;
    lutYW = lutYQ + wY;

    for ( x = wX - 1; x >= 0; x -- )
    {
        lutXQ[ x ] = (int)(DWORD) ( ( x + minX ) * aspectX * 256 );
        lutXW[ x ] = (int)(DWORD) ( ( x + minX ) * splitW  * 256 );
    }

    for ( y = wY - 1; y >= 0; y -- )
    {
        lutYQ[ y ] = (int)(DWORD) ( ( y + minY ) * aspectY * 256 );
        lutYW[ y ] = (int)(DWORD) ( ( y + minY ) * splitQ  * 256 );
    }

    f = MakeCS_Frame( wX + sS, wY + sS );

    if ( !f )
    {
        MemFree( lutXQ );
        return 0;
    }

    dPx = &_PX( f, wX - 1, wY - 1 );

    finalX = INT_MAX;
    firstY = 1;

    for ( y = wY - 1; y >= 0; y --, dPx -= sS )
    {
        int curYQ = lutYQ[ y ];
        int curYW = lutYW[ y ];

        lastX = INT_MAX;
        firstX = 1;

        for ( x = wX - 1; x >= 0; x --, dPx -- )
	    {
		    int fX = (int) ( lutXQ[ x ] - curYW ) >> 8;
            int fY = (int) ( curYQ - lutXW[ x ] ) >> 8;

		    COLORREF col;

		    if ( ( fX < 0 ) || ( fX >= (int) oX ) ) continue;
		    if ( ( fY < 0 ) || ( fY >= (int) oY ) ) continue;

		    col = _PX( o, fX, fY );
		    if ( ! col ) continue;

		    if ( ! dPx[ sD ] )
		    {
			    dPx[ sD ] = ( col == CS_SHADOW_OPAQ ) ? CS_SHADOW_LITE : CS_SHADOW_DARK;

                if ( firstX )
                {
                    if ( f->_rr < (int) ( x + sS ) ) f->_rr = x + sS;
                    firstX = 0;
                }

                if ( firstY )
                {
                    if ( f->_rb < (int) ( y + sS ) ) f->_rb = y + sS;
                    firstY = 0;
                }
		    }

		    *dPx = col;

            lastY = y;
            lastX = x;
	    }

        if ( finalX > lastX ) finalX = lastX;
    }

    f->_rl = finalX;
    f->_rt = lastY;

    MemFree( lutXQ );

    f->_fx = o->_fx * x1 / oX + o->_fy * x2 / oY - minX;
    f->_fy = o->_fx * y1 / oX + o->_fy * y2 / oY - minY;

    f->_rl -= ( ( aaF - (int)(DWORD)( f->_fx - f->_rl ) ) % aaF + aaF ) % aaF;
    f->_rt -= ( ( aaF - (int)(DWORD)( f->_fy - f->_rt ) ) % aaF + aaF ) % aaF;

    f->_rr = ( ( f->_rr - f->_rl ) / aaF + 1 ) * aaF + f->_rl - 1;
    f->_rb = ( ( f->_rb - f->_rt ) / aaF + 1 ) * aaF + f->_rt - 1;

    return f;
}

DWORD mmx_sim[] = { 0x02000000, 0x0300E38F, 0x04000000, 0x0401A3D7, 0x0500E38F, 0x0501A72F, 0x06000000,
                    0x0600CA46, 0x0601A3D7, 0x06008768, 0x0700E38F, 0x0700C1E5, 0x0701A72F, 0x070091A3, 0x08000000 };

CS_FRAME *SrnkCS_Frame( CS_FRAME *o, BYTE aaF )
{
    int lX = o->_x - 1;
	int lY = o->_y - 1;

	int wX = ( o->_rr - o->_rl + 1 ) / aaF;
	int wY = ( o->_rb - o->_rt + 1 ) / aaF;

	int x, y, sX, sY, eX, eY, tX, tY;

    DWORD *sPx, zXY, mTa, mDa;

    CS_FRAME *f = MakeCS_Frame( wX, wY );
    if ( !f ) return 0;

    f->_ix = (int)(DWORD)( ( o->_fx - o->_rl ) / aaF );
	f->_iy = (int)(DWORD)( ( o->_fy - o->_rt ) / aaF );

    if ( aaF == 1 )
    {
        if ( !BitBlt( f->hDC, 0, 0, wX, wY, o->hDC, o->_rl, o->_rt, SRCCOPY ) )
        {
            KillCS_Frame( f );
            return 0;
        }

        return f;
    }

    wX --;
    wY --;

    __asm
    {
        movzx       eax, aaF
        lea         eax, [ offset mmx_sim - 8 + eax * 4 ]
        movd        mm0, [ eax ]

        movzx       eax, WORD PTR [ eax ]
        mov         mTa, eax

        pxor        mm7, mm7
        punpcklwd   mm0, mm7
        movq        mm1, mm0
        punpcklwd   mm1, mm1
        punpcklwd   mm1, mm1

        movq        mm2, mm0
        psrlq       mm2, 40

        psrlq       mm0, 32
        punpcklbw   mm0, mm7
        punpcklwd   mm0, mm0
        punpcklwd   mm0, mm0

//      mm0 - increase option
//      mm1 - multiply factor
//      mm2 - shift count
    }

    sY = wY * aaF + o->_rt;

	for ( y = wY; y != -1; y --, sY -= aaF )
    {
        tY = sY < 0 ? 0 : sY;
        eY = aaF - 1;
        if ( eY + tY > lY ) eY = lY - tY;

        sX = wX * aaF + o->_rl;

        for ( x = wX; x != -1; x --, sX -= aaF )
	    {
            tX = sX < 0 ? 0 : sX;
		    eX = aaF - 1;
		    if ( eX + tX > lX ) eX = lX - tX;

            sPx = &_PX( o, eX + tX, eY + tY );

            zXY = ( lX - eX ) << 2;

            __asm
            {
                mov         eax, [ sPx ]
                pxor        mm3, mm3

                mov         ecx, eY
SR_YL:          mov         edx, eX
SR_XL:
                movd        mm4, [ eax ]
                punpcklbw   mm4, mm7
                paddw       mm3, mm4

                sub         eax, 4
                sub         edx, 1
                jns         SR_XL

                sub         eax, zXY
                sub         ecx, 1
                jns         SR_YL

                paddw       mm3, mm0
                cmp         WORD PTR [ mTa ], 0h
                jz          SKIP_MUL

                pmulhuw     mm3, mm1

    SKIP_MUL:   psrlw       mm3, mm2
                packuswb    mm3, mm7

                movd        mDa, mm3
            }

            _PX( f, x, y ) = mDa;
        }
    }

    __asm emms

	return f;
}

DWORD sum1 = 0, times1 = 0;
DWORD sum2 = 0, times2 = 0;

CS_FRAME *RendCS_Frame( CS_FACTORY *o, float fi )
{
//    TCHAR buffy[ MAX_PATH ];
//    HDC dDC;

	BYTE iMake = 0x60;
	CS_FRAME *n = 0, *f;

/*
    LARGE_INTEGER freq, t0, t1, t2;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&t0);
*/
    f = RoteCS_Frame( o->rndy, &o->head, fi );
    if ( !f ) goto MakeError; iMake ++;

//    QueryPerformanceCounter(&t1);

    n = SrnkCS_Frame( f, o->head.aaFactor );
    if ( !n ) goto MakeError; iMake ++;

//    QueryPerformanceCounter(&t2);

    KillCS_Frame( f );

/*
    dDC = GetDC( 0 );
    sum1 += ( (t1.QuadPart-t0.QuadPart)*1000000)/freq.QuadPart;
    times1 ++;

    sum2 += ((t2.QuadPart-t1.QuadPart)*1000000)/freq.QuadPart;
    times2 ++;

    wsprintf( buffy, TEXT( "P1: %d us      " ), sum1 / times1 );
    TextOut( dDC, 0, 100, buffy, lstrlen( buffy ) );
    wsprintf( buffy, TEXT( "P2: %d us      " ), sum2 / times2 );
    TextOut( dDC, 0, 150, buffy, lstrlen( buffy ) );
    ReleaseDC( 0, dDC );
*/
    return n;

MakeError:

    MessageBox( gm_hWnd, TEXT( "RendCS_Frame crash!" ), 0, MB_ICONHAND );

    KillCS_Frame( n );
    KillCS_Frame( f );

    SetLastError( ERROR_CURSORSHOP( iMake ) );
    return 0;
}
