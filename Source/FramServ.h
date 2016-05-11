#ifndef _FRAMSERV_H_
#define _FRAMSERV_H_

//  ---------------------------------------------------------------------------------------------------------------------------

    #include "SharCode.h"

//  ---------------------------------------------------------------------------------------------------------------------------

    typedef struct
    {
        BITMAPINFOHEADER    bih;        // bitmap info header duh
        HBITMAP             hBM;        // cursor HBITMAP
        DWORD              *dPx;        // pointer to bitmap bits (32 bpp)

        HDC                 hDC;        // hDC in which the bitmap is loaded
        HGDIOBJ             hGO;        // old GDI object from the hDC

        union
        {
            struct
            {
                POINTFLOAT  pFC;        // center float point used by icon shop
                RECT        rBd;        // minimum bounding box
            }
            IS;

            POINT           pSD[ 3 ];   // center, source & destignation points
        }
        UN;
    }
    IS_FRAME;

//  ---------------------------------------------------------------------------------------------------------------------------

    IS_FRAME *MakeIS_FRAME( int f_iX, int f_iY );
    IS_FRAME *LoadIS_FRAME( COAL *path );

    BOOL SaveIS_FRAME( IS_FRAME *f, COAL *path );
    void KillIS_FRAME( IS_FRAME *f );

    void TestIS_FRAME( IS_FRAME *f, int t_iX, int t_iY );

//  ---------------------------------------------------------------------------------------------------------------------------

    #define _x bih.biWidth
    #define _y bih.biHeight

    #define _PX( f, x, y ) ( ( ( f ) -> dPx ) [ ( x ) + ( y ) * ( ( f ) -> _x ) ] )

    #define _rl UN.IS.rBd.left
    #define _rt UN.IS.rBd.top
    #define _rr UN.IS.rBd.right
    #define _rb UN.IS.rBd.bottom

    #define _fx UN.IS.pFC.x
    #define _fy UN.IS.pFC.y

    #define _ix UN.pSD[ 0 ].x
    #define _iy UN.pSD[ 0 ].y

    #define _ps UN.pSD[ 1 ]
    #define _pd UN.pSD[ 2 ]

//  ---------------------------------------------------------------------------------------------------------------------------

#endif // _FRAMSERV_H_
