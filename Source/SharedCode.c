#include "SharedCode.h"

HWND gm_hWnd;
HINSTANCE gm_hInst;

CTRING gm_sCN = TEXT( "2NDHAND_MAINGUI_CLASS" );
CTRING gm_sWN = TEXT( "2ndHand - MainGUI" );

//	----------------------------------------------------------------------------------------------------------------------------

HWND CreateClassWindow( CTRING s_sCN, CTRING s_sWN, WNDPROC s_pProc, DWORD s_dStyle, DWORD s_dStyleEx )
{
	WNDCLASSEX s_xWCX = { 0 };
	{
		s_xWCX.cbSize = sizeof( WNDCLASSEX );
		s_xWCX.hInstance = gm_hInst;
		s_xWCX.lpszClassName = s_sCN;
		s_xWCX.lpfnWndProc = s_pProc;
		s_xWCX.hCursor = LoadCursor( 0, IDC_ARROW );

		if ( s_sWN == gm_sWN ) s_xWCX.hIcon = LoadIcon( gm_hInst, MAKEINTRESOURCE( IDI_POINTER ) );

		if ( !RegisterClassEx( &s_xWCX ) )
		{
			if ( GetLastError() != ERROR_CLASS_ALREADY_EXISTS )	return 0;
		}
	}

	return CreateWindowEx( s_dStyleEx, s_sCN, s_sWN, s_dStyle, 0, 0, 0, 0, 0, 0, 0, 0 );
}

//	----------------------------------------------------------------------------------------------------------------------------

BOOL ERROR_MESSAGE( CTRING eText, BOOL eID )
{
	TCHAR buffy[ MAX_PATH ];

	if ( (char) eID >= 0 )
	{
		wsprintf( buffy, TEXT( "%s error : %02X" ), eText, (BYTE) eID );
		MessageBox( 0, buffy, 0, MB_ICONHAND );
	}

	return eID;
}
