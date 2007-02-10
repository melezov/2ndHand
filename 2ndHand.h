#ifndef _2ND_HAND_H
#define _2ND_HAND_H
#include <winsock2.h>

#define DEFAULT_PORT 0x6CDC;
#define MODE_SERVER 0x1
#define MODE_CLIENT 0x2

#define SWITCH_COUNT 4
static const char *switchTable = "apsc";
#define SWITCH_ADDRESS 0
#define SWITCH_PORT    1
#define SWITCH_SERVER  2
#define SWITCH_CLIENT  3

static volatile bool accepting = true;
static const char *g_sWN = "2nd Hand\nServer";
static const char *g_sCN = "2ND_HAND_SERVER_CLASS";
extern HINSTANCE g_hInst;


//********************************//
//	  Function declaration        //
//********************************//
DWORD WINAPI RunServerHandler( LPVOID s_vParam );

#endif
