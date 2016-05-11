#ifndef _ICONSHOP_H_
#define _ICONSHOP_H_

//  ---------------------------------------------------------------------------------------------------------------------------

    #include "SharCode.h"
    #include "FramServ.h"

//  ---------------------------------------------------------------------------------------------------------------------------

    extern COAL gi_sCN;
    extern COAL gi_sWN;

//  ---------------------------------------------------------------------------------------------------------------------------

    #define WS_ICONSHOP ( WS_POPUP | WS_VISIBLE )
    #define WX_ICONSHOP ( WS_EX_PALETTEWINDOW | WS_EX_NOACTIVATE | WS_EX_LAYERED | WS_EX_TRANSPARENT )

//  ---------------------------------------------------------------------------------------------------------------------------

    #define IS_AAF_MAX              16

    #define IS_WHITE                0x00FFFFFF
    #define IS_ALPHA_MASK           0xFF000000

    #define IS_SHADOW_OPAQ          0xFF000000
    #define IS_SHADOW_DARK          0x5F000000
    #define IS_SHADOW_LITE          0x3F000000

    #define IS_ASPECT_CORRECTION    1.0f
    #define IS_ANGLE_APPROACH       8

//  ---------------------------------------------------------------------------------------------------------------------------

    typedef struct
    {
        DWORD   resId;          // resource Id
        DWORD   aaFactor;       // anti-aliasing factor x by x
        SIZE    destSize;       // preferred cursor size
        float   borderWidth;    // border width in destignation pixels
        float   shadowDist;     // shadow x and y distance in destignation pixels
        float   plasmaChaos;    // plasma dispersion factor
        DWORD   randSeed;       // random seed for color dispersion & co.
        DWORD   buffSize;       // frame buffer size
    }
    IS_HEADER;

//  ---------------------------------------------------------------------------------------------------------------------------

    typedef struct
    {
        IS_HEADER           head;       // cursor factory information
        HWND                hwnd;       // cursor window

        IS_FRAME           *rndy;       // rendered bordered plasma fractal
    }
    IS_FACTORY;

//  ---------------------------------------------------------------------------------------------------------------------------

    IS_FACTORY *MakeIS_FACTORY( IS_HEADER *h );
    void KillIS_FACTORY( IS_FACTORY *f );

    void Move2ndHand( HWND i_hWnd, IS_FRAME *f, POINT *i_pCur );

//  ---------------------------------------------------------------------------------------------------------------------------

    DWORD MulHalfAdd( DWORD i_dAAF, float i_fVal );

    //  ---------------------------------------------------------------------------------------------------------------------------

#endif // _ICONSHOP_H_
