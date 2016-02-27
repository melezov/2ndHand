#include "NaviCon.h"

//0x2900 PORT

HINSTANCE   n_hInst;
SOCKET      n_sSocket;

//Test stuff
int         n_iWhatAmI;
char        n_sWhatAmI[20];
HWND        n_hWindow;


void ooo( int iPortNumber, char *cServerAddr, HWND hWindow )
{
    n_hWindow = hWindow;
    if( cServerAddr )
    {
        if( ClientInit( iPortNumber, cServerAddr, hWindow ) )
        {
            MessageBox( 0, "Failed to initialize client", "ERROR!", MB_OK );
        }
    }
    else
    {
        if( ServerInit( iPortNumber, hWindow ) )
        {
            MessageBox( 0, "Failed to initialize server", "ERROR!", MB_OK );
        }
    }
}

int WINAPI _DllMainCRTStartup( HINSTANCE t_hInst, DWORD n_dReason, void *n_vReserved )
{
	if ( n_dReason == DLL_PROCESS_ATTACH )
	{
		if ( !n_hInst ) n_hInst = t_hInst;
	}

	return 1;
}

int ServerInit( int iPortNumber, HWND hWindow )
{
    WSADATA          wdData;
    sockaddr_in      addrServer;
    int              iAddrLen = sizeof( sockaddr_in );
    DWORD            myData=13 ;

    wsprintf( n_sWhatAmI, "server" );
    n_iWhatAmI = 1;

    WSAStartup( 0x101 , &wdData );

    n_sSocket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

    addrServer.sin_port        = htons( iPortNumber );
    addrServer.sin_family      = AF_INET;
    addrServer.sin_addr.s_addr = htonl( INADDR_ANY );
    if( bind( n_sSocket, ( struct sockaddr * ) &addrServer, sizeof( struct sockaddr_in ) ) == -1 )
    {
        int i = WSAGetLastError();
        MessageBox( 0, "Failed on BIND.\nOK to exit.", n_sWhatAmI, MB_OK );
        return 0x101;
    }

    if( listen( n_sSocket, 0 ) == -1 )
    {
        MessageBox( 0, "Failed on LISTEN.\nOK to exit.", n_sWhatAmI, MB_OK );
        return 0x102;
    }

    if( WSAAsyncSelect( n_sSocket, hWindow, WM_NAVISOCKETMESSAGE, FD_ACCEPT | FD_CLOSE | FD_READ ) )
    {
        MessageBox( 0, "Failed on ASYNC.\nOK to exit.", n_sWhatAmI, MB_OK );
        return 0x103;
    }

    return 0;
}

int ClientInit( int iPortNumber, char *cServerAddr, HWND hWindow )
{
    WSADATA          wdData;
    sockaddr_in      addrServer,
                     addrClient;
    /*TEST*/
    DWORD testdata[100];
    int i;
    for (i=0;i<100;testdata[i++]=i);
    /*/TEST*/

    wsprintf( n_sWhatAmI, "client" );
    n_iWhatAmI = 0;

    WSAStartup( 0x101 , &wdData );

    n_sSocket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

    addrClient.sin_family       = AF_INET;
    addrClient.sin_addr.s_addr  = htonl( INADDR_ANY );
    addrClient.sin_port         = htons( iPortNumber );

    //podesiti linger?
    addrServer.sin_family      = AF_INET;
    addrServer.sin_addr.s_addr = inet_addr( cServerAddr );
    addrServer.sin_port        = htons( iPortNumber );

    if ( connect( n_sSocket, (struct sockaddr *) &addrServer, sizeof( addrServer ) ) == -1 )
    {
        MessageBox( NULL, "Failed to connect", n_sWhatAmI, MB_OK );
        return 0x201;
    }

    if( WSAAsyncSelect( n_sSocket, hWindow, WM_NAVISOCKETMESSAGE, FD_CONNECT | FD_ACCEPT ) )
    {
        MessageBox( 0, "Failed on ASYNC.\nOK to exit.", n_sWhatAmI, MB_OK );
        return 0x103;
    }

    /*TEST*/
    MessageSendData( n_sSocket, SECOND_HAND_MESSAGE( SECOND_HAND_ID_FILE, 400 ), testdata );
    /*/TEST*/

    return 0;
}


//TODO: ime, procedure, greske i slicno
int SendMouse ( int x, int y )
{
    DWORD poruka = SECOND_HAND_MESSAGE( SECOND_HAND_ID_MOUSE, SECOND_HAND_MOUSE_MESSAGE( x, y ) );

    MessageSendData( n_sSocket, poruka, NULL ) ;

    return 0;
}

int SendFile ( char *sFileName )
{
    HFILE    hFile;
    OFSTRUCT ofFileInfo;
    DWORD    poruka, iBytesRead;
    BYTE     *pbContent;

    if ( !(hFile = OpenFile( sFileName, &ofFileInfo, OF_READ)) )
    {
        return 0x501;
    }

    poruka = GetFileSize( (HANDLE) hFile, NULL );
    pbContent = VirtualAlloc( NULL, poruka, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE );

    ReadFile( (HANDLE) hFile, pbContent, poruka, &iBytesRead, NULL);
    CloseHandle( (HANDLE) hFile );

    if( poruka != iBytesRead )
    {
        return 0x502;
    }

    poruka = SECOND_HAND_MESSAGE( SECOND_HAND_ID_FILE, poruka );
    MessageSendData( n_sSocket, poruka, (DWORD *) pbContent );

    return 0;
}

int SendData ( DWORD iSize, DWORD *sData )
{
    DWORD    poruka;

    poruka = SECOND_HAND_MESSAGE( SECOND_HAND_ID_FILE, iSize );
    MessageSendData( n_sSocket, poruka, sData );

    return 0;
}

int MessageSendData ( SOCKET sSocketID, DWORD dwMessage, DWORD *dwData )
{
    if ( send( sSocketID, (const char *) &dwMessage, sizeof( DWORD ), 0 ) == -1 )
    {
        return 0x401;
    }

    if ( SECOND_HAND_MESSAGE_ID( dwMessage ) == SECOND_HAND_ID_FILE )
    {
        if( send( sSocketID, (const char *) dwData, SECOND_HAND_MESSAGE_BODY( dwMessage ), 0 ) == -1 )
        {
            return 0x402;
        }
    }

    return 0;
}

int MessageGetData ( SOCKET sSocketID, DWORD *dwMessage, DWORD **dwData )
{
    *dwData = NULL;
    if( recv( sSocketID, (char *) dwMessage, sizeof( DWORD ), 0 ) == -1 )
    {
       return 0x301;
    }

    if ( SECOND_HAND_MESSAGE_ID( *dwMessage ) == SECOND_HAND_ID_FILE )
    {
        if ( !(*dwData = VirtualAlloc( NULL, SECOND_HAND_MESSAGE_BODY( *dwMessage ), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE )) )
        {
            return 0x303;
        }
        if ( recv( sSocketID, (char *) *dwData, SECOND_HAND_MESSAGE_BODY( *dwMessage ), 0 ) == -1 )
        {
            return 0x302;
        }
    }

    return 0;
}

int ProcessNAVIMessage( SOCKET sSocket, LPARAM lEvent )
{
    int          iAddrLen = sizeof( sockaddr_in );
    sockaddr_in  addrServer;

    if( WSAGETSELECTERROR( lEvent ) )
    {
        //WSAECONNABORTED - kada nasilno pukne veza
        MessageBox( NULL, "Greska, zatvaram socket", n_sWhatAmI, MB_OK );
        closesocket( sSocket );
    }

    switch( WSAGETSELECTEVENT( lEvent) )
    {
        case FD_ACCEPT:
            MessageBeep(MB_OK);
            accept( sSocket, (struct sockaddr *) &addrServer, &iAddrLen);
        break;

        case FD_CONNECT:
            MessageBeep(MB_OK);
            MessageBox( NULL, "Connected", n_sWhatAmI, MB_OK );
        break;

        case FD_CLOSE:
            MessageBox( NULL, "Closed", n_sWhatAmI, MB_OK );
            closesocket( sSocket );
        break;

        case FD_READ:
        {
            DWORD   dwID,
                    *dwData = NULL,
                    ID;

            MessageGetData( sSocket, &dwID, &dwData );
            ID = SECOND_HAND_MESSAGE_ID( dwID );
            switch( ID )
            {
                case SECOND_HAND_ID_MOUSE:
                {
                    int x = 0,
                        y = 0;

                    x  = SECOND_HAND_MOUSE_X( SECOND_HAND_MESSAGE_BODY( dwID ) );
                    y  = SECOND_HAND_MOUSE_Y( SECOND_HAND_MESSAGE_BODY( dwID ) );
                    PostMessage( n_hWindow, WM_NAVIMESSAGE_MOUSE, x, y );
                }
                break;

                case SECOND_HAND_ID_FILE:
                {
                    int    iFileLen  = 0;

                    iFileLen = SECOND_HAND_MESSAGE_BODY( dwID );
                    PostMessage( n_hWindow, WM_NAVIMESSAGE_FILE, iFileLen, (LPARAM)dwData );
                }
                break;

                case SECOND_HAND_ID_KEY:
                {
                    PostMessage( n_hWindow, WM_NAVIMESSAGE_KEY, SECOND_HAND_MESSAGE_BODY( dwID ), 0 );
                }
            }
        }
        break; //FD_READ
    }

    return 0;
}
