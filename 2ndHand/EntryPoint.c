#include "EntryPoint.h"

HWND ge_hWnd;
HINSTANCE ge_hInst;

CTRING ge_sCN = TEXT( "2NDHAND_MAINGUI_CLASS" );
CTRING ge_sWN = TEXT( "2ndHand - MainGUI" );

TCHAR ge_sMI[ MAX_PATH ];

HMODULE ge_hDll;
HWND ge_hBox;

LRESULT CALLBACK e_pProc( HWND e_hWnd, UINT e_uMsg, WPARAM e_wParam, LPARAM e_lParam )
{
	if ( e_uMsg == WM_NCCREATE )
	{
		ge_hWnd = e_hWnd;

		ge_hDll = LoadLibrary( "RichEd20.dll" );
		if ( !ge_hDll ) return 0;

		ge_hBox = CreateWindow( TEXT( "RichEdit20A" ), 0, WS_BORDER | WS_VISIBLE | WS_CHILD | ES_AUTOVSCROLL | ES_MULTILINE | ES_NOHIDESEL | ES_READONLY, 0, 0, 0, 0, e_hWnd, (HMENU) 0x10, 0, 0 );
		SendMessage( ge_hBox, EM_SETBKGNDCOLOR, 0, 0x000000 );
		if ( !ge_hBox ) return 0;

		OutputText( OT_MAINGUI, TEXT( "Created!" ) );

		TrayParse( WM_TRAYIDOL, NIM_ADD );
		HookInit();
	}

	else if ( e_uMsg == WM_SHOWWINDOW )
	{
		OutputText( OT_MAINGUI, e_wParam ? TEXT( "Showing ..." ) : TEXT( "Hiding ..." ) );
	}

	else if ( e_uMsg == WM_SIZE )
	{
		RECT eBounds;

		if ( e_wParam == SIZE_MINIMIZED )
		{
			ShowWindow( e_hWnd, SW_HIDE );
			return 0;
		}

		GetClientRect( e_hWnd, &eBounds );
		SetWindowPos( ge_hBox, HWND_TOP, 0, 0, eBounds.right, eBounds.bottom, SWP_SHOWWINDOW );
	}

	else if ( e_uMsg == WM_CLOSE )
	{
		OutputText( OT_MAINGUI, TEXT( "Closing!" ) );
	}

	else if ( e_uMsg == WM_DESTROY )
	{
		TrayParse( WM_TRAYIDOL, NIM_DELETE );

		OutputText( OT_MAINGUI, TEXT( "Destroyed!" ) );
		PostQuitMessage( 0 );
	}

	else if ( e_uMsg == WM_TRAYCREA )
	{
		TrayParse( WM_TRAYIDOL, NIM_ADD );
		return 0;
	}

	else if ( e_uMsg == WM_TRAYIDOL )
	{
		TrayParse( e_wParam, e_lParam );
		return 0;
	}

	return DefWindowProc( e_hWnd, e_uMsg, e_wParam, e_lParam );
}

int WinMainCRTStartup()
{
	ge_hInst = GetModuleHandle( 0 );

	if ( !CreateClassWindow( ge_sCN, ge_sWN, e_pProc, WS_ENTRYPOINT, WX_ENTRYPOINT ) ) return ERROR_ENTRYPOINT( 0 );

	SetWindowPos( ge_hWnd, HWND_TOP, 100, 100, 250, 800, SWP_SHOWWINDOW );

	while( 1 )
	{
		MSG e_xMsg;
	 	int e_iMsg = GetMessage( &e_xMsg, 0, 0, 0 );

		if ( !( ++ e_iMsg ) ) continue;
		if ( !( -- e_iMsg ) )
		{
			FreeLibrary( ge_hDll );
			return (int) e_xMsg.wParam;
		}

		TranslateMessage( &e_xMsg );
		DispatchMessage( &e_xMsg );
	}
}
