#ifndef _SHARXTRA_H_
#define _SHARXTRA_H_

//  ---------------------------------------------------------------------------------------------------------------------------

    #include "..\Source\SharAorW.h"
    #include "..\Source\SharDefs.h"

//  ---------------------------------------------------------------------------------------------------------------------------

    #ifndef _SHARCODE_H_

        #define CoGetMalloc IMP ## CoGetMalloc ## @8
        #define GetModuleHandle IMP ## GetModuleHandle ## AW ## @4
        #define GetMessage IMP ## GetMessage ## AW ## @16
        #define DispatchMessage IMP ## DispatchMessage ## AW ## @4
        #define TranslateMessage IMP ## TranslateMessage@4

    #endif // _SHARCODE_H

//  ---------------------------------------------------------------------------------------------------------------------------

#endif // _SHARXTRA_H_
