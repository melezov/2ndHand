#include "MainGUI.h"

HWND CreateClassWindow( HINSTANCE s_hInst, LPCSTR s_sCN, LPCSTR s_sWN, WNDPROC s_pProc, DWORD s_dStyle, DWORD s_dStyleEx )
{
	WNDCLASSEX s_xWCX = { 0 };
	{
		s_xWCX.cbSize = sizeof( WNDCLASSEX );
		s_xWCX.hInstance = s_hInst;
		s_xWCX.lpszClassName = s_sCN;
		s_xWCX.lpfnWndProc = s_pProc;

		if ( !RegisterClassEx( &s_xWCX ) ) return 0;
	}

	return CreateWindowEx( s_dStyleEx, s_sCN, s_sWN, s_dStyle, 0, 0, 0, 0, 0, 0, 0, 0 );
}

int MessagePump()
{
    MSG m_xMsg;
	BOOL t_i;

	do
	{
		if ( ( t_i = GetMessage( &m_xMsg, 0, 0, 0 ) ) <= 0 ) continue;

		TranslateMessage( &m_xMsg );
		DispatchMessage( &m_xMsg );
	}
	while( t_i );

    return (int) m_xMsg.wParam;
}
