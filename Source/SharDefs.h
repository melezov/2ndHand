#ifndef _SHARDEIS_FRAME_H_
#define _SHARDEIS_FRAME_H_

//  ---------------------------------------------------------------------------------------------------------------------------

    #define MAX_STRING 0x80

//  ---------------------------------------------------------------------------------------------------------------------------

    #define WM_2NDHAND 0x400

    #define WM_TRAYIDOL ( WM_2NDHAND + 0x0 )
    #define WM_RATHOOK  ( WM_2NDHAND + 0x1 )
    #define WM_KEYHOOK  ( WM_2NDHAND + 0x2 )

//  ---------------------------------------------------------------------------------------------------------------------------

    #define IDI_POINTER 0x10
    #define IDI_PICKER  0x20
    #define IDI_DENIED  0x30

    #define IDB_POINTER ( IDI_POINTER + 0x01 )
    #define IDB_PICKER  ( IDI_PICKER  + 0x01 )
    #define IDB_DENIED  ( IDI_DENIED  + 0x01 )

    #define IDX_LAST IDB_DENIED

//  ---------------------------------------------------------------------------------------------------------------------------

    #define ERROR_SHARCODE( e ) ( 0xE0000000 + ( e ) )
    #define ERROR_MAINGXUI( e ) ( 0xE0001000 + ( e ) )
    #define ERROR_TRAYIDOL( e ) ( 0xE0001100 + ( e ) )
    #define ERROR_NAVYHOOK( e ) ( 0xE0001200 + ( e ) )
    #define ERROR_ICONSHOP( e ) ( 0xE0002000 + ( e ) )
    #define ERROR_FRAMSERV( e ) ( 0xE0002100 + ( e ) )
    #define ERROR_PLASREND( e ) ( 0xE0002200 + ( e ) )
    #define ERROR_FRAMREND( e ) ( 0xE0002300 + ( e ) )
    #define ERROR_TESTPROC( e ) ( 0xE0003000 + ( e ) )

//  ---------------------------------------------------------------------------------------------------------------------------

#endif // _SHARDEIS_FRAME_H_
