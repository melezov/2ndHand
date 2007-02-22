#include "NaviCon.h"

HINSTANCE n_hInst;

int WINAPI _DllMainCRTStartup( HINSTANCE t_hInst, DWORD n_dReason, void *n_vReserved )
{
	if ( n_dReason == DLL_PROCESS_ATTACH )
	{
		if ( !n_hInst ) n_hInst = t_hInst;
	}

	return 1;
}
