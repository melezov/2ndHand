#include <windows.h>

const char * w_sCN = "2ND_HAND_CLASS";
const char * w_sWN = "Second Hand";

HICON w_hIcon, w_hXcon;
WORD w_wX, w_wY;

bool w_oActive = false;

LRESULT CALLBACK WndProc( HWND w_hWnd, UINT w_uMsg, WPARAM w_wParam, LPARAM w_lParam )
{
	if ( w_uMsg == WM_PAINT )
	{
		PAINTSTRUCT t_PS;

		HDC t_DC = BeginPaint( w_hWnd, &t_PS );

		for ( int x = 0; x < w_wX; x ++ )
		for ( int y = 0; y < w_wY; y ++ )
		{
			SetPixel( t_DC, x, y, RGB( 255 - ( x + 1 ) * 125 / w_wX, 255 - ( y + 1 ) * 125 / w_wY , 0 ) );
		}

//		DrawIconEx( t_DC, 0, 0, w_hIcon, w_wX, w_wY, 0, 0, DI_NORMAL | DI_COMPAT );
		EndPaint( w_hWnd, &t_PS );
	}

	else if ( w_uMsg == WM_SIZE )
	{
		w_wX = LOWORD( w_lParam );
		w_wY = HIWORD( w_lParam );

		if ( w_wX < 4 ) w_wX = 4;
		if ( w_wY < 4 ) w_wY = 4;

		HDC t_DC = CreateCompatibleDC( 0 );
		HBITMAP t_BM = CreateCompatibleBitmap( t_DC, w_wX, w_wY );

		SelectObject( t_DC, t_BM );

		for ( int x = 0; x < w_wX; x ++ )
		for ( int y = 0; y < w_wY; y ++ )
		{
			SetPixelV( t_DC, x, y, 0 );
		}

		DrawIconEx( t_DC, 0, 0, w_hIcon, w_wX, w_wY, 0, 0, DI_NORMAL | DI_COMPAT );

		HRGN t_RG0 = CreateRectRgn( 0, 0, w_wX, w_wY );

		for ( int x = 0; x < w_wX; x ++ )
		{
			for ( int y = 0; y < w_wY; y ++ )
			{
				DWORD t_PX = GetPixel( t_DC, x, y ) & 0xffffff;

				if ( t_PX == 0 )
				{
					HRGN t_RG = CreateRectRgn( x, y, x + 1, y + 1 );
					CombineRgn( t_RG0, t_RG0, t_RG, RGN_DIFF );
					DeleteObject( t_RG );
				}
			}
		}

		InvalidateRect( w_hWnd, 0, 0 );

		SetWindowRgn( w_hWnd, t_RG0, 1 );

		DeleteDC( t_DC );
		DeleteObject( t_BM );
	}

	else if ( w_uMsg == WM_LBUTTONDOWN )
	{
		SendMessage( w_hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0 );
	}

	else if ( w_uMsg == WM_CREATE )
	{
		SendMessage( w_hWnd, WM_USER, NIM_ADD, 0 );
	}

	else if ( w_uMsg == WM_DESTROY )
	{
		SendMessage( w_hWnd, WM_USER, NIM_DELETE, 0 );

		PostQuitMessage( 0 );
	}

	else if ( w_uMsg == 0x20A ) // WM_MOUSEWHEEL
	{
		short int t_iRot = ( (short int) HIWORD( w_wParam ) ) / 120;

		RECT t_RC;
		GetWindowRect( w_hWnd, &t_RC );

		SetWindowPos( w_hWnd, HWND_TOPMOST, t_RC.left + t_iRot, t_RC.top + t_iRot, w_wX - t_iRot, w_wY - t_iRot, SWP_SHOWWINDOW );
	}

	else if ( w_uMsg == WM_USER )
	{
		if ( w_lParam == WM_MOUSEMOVE )
		{
			SetForegroundWindow( w_hWnd );
		}

		else if ( w_lParam == WM_LBUTTONDOWN )
		{
			RECT t_RC;
			GetWindowRect( w_hWnd, &t_RC );

			DWORD t_dX = GetSystemMetrics( SM_CXSCREEN );
			DWORD t_dY = GetSystemMetrics( SM_CYSCREEN );

			SetWindowPos( w_hWnd, HWND_TOPMOST, ( t_dX - w_wX ) / 2, ( t_dY - w_wY ) / 2, w_wX, w_wY, w_oActive ? SWP_SHOWWINDOW : SWP_HIDEWINDOW );
		}

		else if ( w_lParam == WM_LBUTTONDBLCLK )
		{
			w_oActive = !w_oActive;

			SendMessage( w_hWnd, WM_USER, NIM_MODIFY, 0 );
		}

		else if ( w_lParam == WM_RBUTTONDOWN )
		{
			HMENU w_hTM = CreatePopupMenu();

			AppendMenu( w_hTM, MF_STRING, WM_DESTROY, "Exit 2nd Hand" );

			POINT cPos;
			GetCursorPos( &cPos );

			SetForegroundWindow( w_hWnd );
			int w_iTM = TrackPopupMenuEx( w_hTM, TPM_RETURNCMD | TPM_RIGHTALIGN | TPM_RIGHTBUTTON |/* TPM_NOANIMATION*/0x4000L| TPM_BOTTOMALIGN|TPM_NONOTIFY|TPM_VERTICAL, cPos.x, cPos.y, w_hWnd, 0 );

			DestroyMenu( w_hTM );

			if ( w_iTM == WM_DESTROY ) DestroyWindow( w_hWnd );
		}

		else if ( ( w_lParam == NIM_ADD ) || ( w_lParam == NIM_DELETE ) )
		{
			NOTIFYICONDATA w_cNID = { 0 };
			{
				w_cNID.cbSize	= sizeof( NOTIFYICONDATA );
				w_cNID.hWnd		= w_hWnd;
				w_cNID.hIcon	= w_oActive ? w_hIcon : w_hXcon;
				w_cNID.uFlags	= NIF_MESSAGE | NIF_ICON | NIF_TIP;

				lstrcpy( w_cNID.szTip, w_sWN );
				w_cNID.uCallbackMessage = WM_USER;
			}

			SendMessage( w_hWnd, WM_USER, 0, WM_LBUTTONDOWN );

			Shell_NotifyIcon( (DWORD) w_wParam, &w_cNID );
		}
	}

	else
	{
		static DWORD WM_TRAYCREA = RegisterWindowMessage( "TaskbarCreated" );

		if ( ( w_uMsg == WM_TRAYCREA ) || ( w_uMsg == WM_CREATE ) )
		{
			SendMessage( w_hWnd, WM_USER, NIM_ADD, 0 );
		}

		else return DefWindowProc( w_hWnd, w_uMsg, w_wParam, w_lParam );
	}

	return 0;
}

int WINAPI WinMain( HINSTANCE w_hInst, HINSTANCE x_hPrev, LPSTR x_sCmdLn, int x_nShow )
{
	WNDCLASSEX w_MW = { 0 };
	{
		w_MW.cbSize = sizeof( WNDCLASSEX );
		w_MW.hInstance = w_hInst;
		w_MW.lpszClassName = w_sCN;
		w_MW.lpfnWndProc = WndProc;
		w_MW.hCursor = LoadCursor( 0, IDC_ARROW );
	}

	ATOM t_w = RegisterClassEx( &w_MW );
	if ( !t_w ) return 1;

	w_hIcon = LoadIcon( w_hInst, (LPCSTR) 0x100 );
	if ( !w_hIcon ) return 2;

	w_hXcon = LoadIcon( w_hInst, (LPCSTR) 0x101 );
	if ( !w_hXcon ) return 2;

	HWND w_hWnd = CreateWindowEx( WS_EX_TOOLWINDOW, (LPCSTR) t_w, (LPCSTR) w_sWN, WS_POPUP, 0, 0, 32, 32, 0, 0, w_hInst, 0 );
	if ( !w_hWnd ) return 3;

	MSG w_MX;

	while( 1 )
	{
		BOOL t_i = GetMessage( &w_MX, 0, 0, 0 );

		if ( t_i == -1 ) continue;
		if ( t_i ==  0 ) break;

		TranslateMessage( &w_MX );
		DispatchMessage( &w_MX );
	}

	return 0;
}
