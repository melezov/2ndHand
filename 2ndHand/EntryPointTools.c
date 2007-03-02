#include "EntryPoint.h"

extern HINSTANCE ge_hInst;

HWND CreateClassWindow( CTRING s_sCN, CTRING s_sWN, WNDPROC s_pProc, DWORD s_dStyle, DWORD s_dStyleEx )
{
	WNDCLASSEX s_xWCX = { 0 };
	{
		s_xWCX.cbSize = sizeof( WNDCLASSEX );
		s_xWCX.hInstance = ge_hInst;
		s_xWCX.lpszClassName = s_sCN;
		s_xWCX.lpfnWndProc = s_pProc;
		s_xWCX.hCursor = LoadCursor( 0, IDC_ARROW );

		if ( !RegisterClassEx( &s_xWCX ) )
		{
			if ( GetLastError() != ERROR_CLASS_ALREADY_EXISTS )	return 0;
		}
	}

	return CreateWindowEx( s_dStyleEx, s_sCN, s_sWN, s_dStyle, 0, 0, 0, 0, 0, 0, 0, 0 );
}
