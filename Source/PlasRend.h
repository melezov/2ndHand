#ifndef _PLASREND_H_
#define _PLASREND_H_

//  ---------------------------------------------------------------------------------------------------------------------------

    #include "IconShop.h"

//  ---------------------------------------------------------------------------------------------------------------------------

    #define IS_PLASMA_MAX_COLORS 0xFF

//  ---------------------------------------------------------------------------------------------------------------------------

    IS_FRAME *MakeIS_PLASMA( IS_FRAME *o, IS_HEADER *h );
    IS_FRAME *BlotIS_PLASMA( DWORD rad, DWORD inCol );

//  ---------------------------------------------------------------------------------------------------------------------------

#endif // _PLASREND_H_
