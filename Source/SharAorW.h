#ifndef _SHARAORW_H_
#define _SHARAORW_H_

//  ---------------------------------------------------------------------------------------------------------------------------

//    #define UNICODE 1   // Comment out for ANSI version
//    #define IMPORT 1    // Comment out for direct API calls

//  ---------------------------------------------------------------------------------------------------------------------------

    #ifdef UNICODE

        #define _UNICODE UNICODE
        #define STRINGS UNICODE

        #define dss dus
        #define AW W

    #else

        #define STRINGS ANSI

        #define dss db
        #define AW A

    #endif // UNICODE

//  ---------------------------------------------------------------------------------------------------------------------------

    #ifdef IMPORT

        #define IMP __imp__

    #else

        #define IMP _

    #endif // IMPORT

//  ---------------------------------------------------------------------------------------------------------------------------

#endif // _SHARAORW_H_
