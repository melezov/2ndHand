#include "2ndHand.h"

int InitServer( const int s_iPort )
{
	SOCKET sSocketID, sNewSocketID;
    struct sockaddr_in myAddr, cliAddr;
    int    sinLength = sizeof( sockaddr_in );

    sSocketID = socket( AF_INET, SOCK_STREAM, 0 );
	{
		myAddr.sin_family      = AF_INET;
		myAddr.sin_addr.s_addr = htonl( INADDR_ANY );
		myAddr.sin_port        = htons( s_iPort );
	}

    if ( bind( sSocketID, (struct sockaddr *) &myAddr, sinLength ) == -1 ) return 0x300;

	if ( listen( sSocketID, 0 ) == -1 ) return 0x301;

	CreateThread( 0, 0, (LPTHREAD_START_ROUTINE) RunServerHandler, (LPVOID) sSocketID, 0, 0 );

	while( accepting )
	{
		if ( ( sNewSocketID = accept( sSocketID, ( struct sockaddr * ) &cliAddr, &sinLength ) ) == -1 )
		{
			int t_LE = WSAGetLastError();

			if ( t_LE == WSAEINTR ) break; else return 0x302;
		}
        else
        {
        }
	}

    return 0;
}

HINSTANCE g_hInst;

int WINAPI WinMain( HINSTANCE w_hInst, HINSTANCE x_hPrev, LPSTR w_sCmdLn, int x_nShow )
{
    g_hInst = w_hInst;

	char *address = 0;
	int port = 0;
	char mode = 0;

	for ( char *switchSeek = w_sCmdLn; *switchSeek != 0; switchSeek ++ )
	{
		if ( *switchSeek == '/' )
		{
			int switchID = (int) ( strchr( switchTable, switchSeek[ 1 ] ) - switchTable );

			switch ( switchID )
			{
				case SWITCH_ADDRESS :
				{
					address = switchSeek + 2;

					const char *t_space = strchr( address, ' ' );
					if ( t_space ) address[ t_space - address ] = 0;

					break;
				}

				case SWITCH_PORT :
				{
					port = atoi( switchSeek + 2 );
					break;
				}

				case SWITCH_SERVER :
				{
					mode |= MODE_SERVER;
					break;
				}

				case SWITCH_CLIENT :
				{
					mode |= MODE_CLIENT;
					break;
				}

				default :
				{
					return 0x100;
				}
			}
		}
	}

	if ( mode == 0 ) return 0x101;

	if ( ( mode & MODE_CLIENT ) && ( address == 0 ) ) return 0x102;

	if ( port == 0 ) port = DEFAULT_PORT;

    WSADATA wsaData;
    if ( WSAStartup( 0x101, &wsaData ) ) return 0x200;

    if ( mode & MODE_SERVER ) InitServer( port );

	WSACleanup();

    return 0;
}
