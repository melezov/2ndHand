#ifndef _NAVICON_H_
#define _NAVICON_H_

	#ifdef _SHAREDINCLUDES_H_

	    __declspec( dllimport ) void ooo                ( int iPortNumber, char *cServerAddr, HWND hWindow  );    // @1 NONAME
        __declspec( dllimport ) int  ProcessNAVIMessage ( SOCKET sSocket, LPARAM lEvent );                        // @2 NONAME
        __declspec( dllimport ) int  SendMouse          ( int x, int y );                                         // @3 NONAME

	#endif // _SHAREDINCLUDES_H_

   	#include "..\\SharedIncludes.h"

	typedef struct
	{
		short   sin_family;
		u_short sin_port;
		struct  in_addr sin_addr;
		char    sin_zero[ 8 ];
	}
	sockaddr_in;

    //za ubuduce ?
    typedef struct
    {
        char    sFileName[ 1024 ];
        //???
    }
    second_hand_file_info;

    #define WM_NAVISOCKETMESSAGE  ( WM_USER + 0x200 )
    #define WM_NAVIMESSAGE_MOUSE  ( WM_USER + 0x201 )   //lParam = x, wParam = y
    #define WM_NAVIMESSAGE_KEY    ( WM_USER + 0x202 )   //lParam = key
    #define WM_NAVIMESSAGE_FILE   ( WM_USER + 0x205 )   //lParam = size, wParam = data
    #define WM_NAVIMESSAGE_EXIT   ( WM_USER + 0x209 )

    #define SECOND_HAND_ID_MOUSE   1
    #define SECOND_HAND_ID_KEY     2
    #define SECOND_HAND_ID_FILE    5
    #define SECOND_HAND_ID_EXIT    9

    #define SECOND_HAND_MESSAGE_WIDTH           8
    #define SECOND_HAND_MESSAGE_ID( x )		    ( x & ( ( 1 << SECOND_HAND_MESSAGE_WIDTH ) - 1 ) )
    #define SECOND_HAND_MESSAGE_BODY( x )	    ( x >> SECOND_HAND_MESSAGE_WIDTH )
    #define SECOND_HAND_MESSAGE( x, y )		    ( x | ( y << SECOND_HAND_MESSAGE_WIDTH ) )

    //Mouse message  0xIDxxxyyy
    #define SECOND_HAND_MOUSE_MESSAGE( x, y )   ( x << 12 | ( y ) )
    #define SECOND_HAND_MOUSE_X( x )            ( x >> 12 )
    #define SECOND_HAND_MOUSE_Y( x )            ( x & 0x000FFF )



    int     ServerInit         ( int iPortNumber, HWND hWindow );
    int     ClientInit         ( int iPortNumber, char *cServerAddr, HWND hWindow );
    int     MessageSendData    ( SOCKET sSocketID, DWORD dwMessage, DWORD *dwData );
    int     MessageGetData     ( SOCKET sSocketID, DWORD *dwMessage, DWORD *dwData );

#endif // _NAVICON_H_
