#include "ClientSpace.h"

CTRING gc_sCN = TEXT( "2NDHAND_PREVIEW_CLASS" );
CTRING gc_sWN = TEXT( "2ndHand - Cursor Preview" );

#define CP_OPTIONS_NUMBER 7
#define CP_OPTIONS_INTEGERS 4

STRING c_sOptions[ CP_OPTIONS_NUMBER << 1 ] =
{
	TEXT( "Rotation frames:" ),			(STRING)  32,
	TEXT( "Preferred Width:" ),			(STRING)  36,
	TEXT( "Preferred Height:" ),		(STRING)  27,
	TEXT( "Quality (AA factor):" ),		(STRING)   3,
	TEXT( "Border thickness:" ),		(STRING) 125,
	TEXT( "Shadow distance:" ),			(STRING) 300,
	TEXT( "Gradient chaos:" ),			(STRING)  60
};

#define CP_BOX_HEIGHT 18

#define CP_HOR_ALIGN( x ) ( ( x >> 1 ) + CP_BOX_HEIGHT )
#define CP_VER_ALIGN( x ) ( ( x - 6 * CP_BOX_HEIGHT ) * ops / ( CP_OPTIONS_NUMBER - 1 ) + CP_BOX_HEIGHT )

#define CP_PREVIEW_MINX 250
#define CP_PREVIEW_MINY 300

HWND gc_hWnd;
HWND o_hWnd[ CP_OPTIONS_NUMBER ];
HWND b_hWnd;

CS_DUMP *g_cPreview = 0;

BOOL CheckEditBox( BYTE ops )
{
	DWORD i;
	TCHAR buffy[ MAX_PATH ];

	GetWindowText( o_hWnd[ ops ], buffy, MAX_PATH );

	i = atoi( buffy );

	if ( i > 0 )
	{
		wsprintf( buffy, TEXT( "%d" ), i );
		c_sOptions[ ( ops << 1 ) + 1 ] = (STRING) 0 + i;
	}

	return i;
}

CS_DUMP *Create2ndHand()
{
	CS_HEADER org;
	BYTE ops;

	org.cursorId = 0x50;
	org.randSeed = 0;

	for ( ops = 0; ops < CP_OPTIONS_NUMBER; ops ++ )
	{
		WORD i = CheckEditBox( ops );

		if ( i <= 0 ) return 0;

		switch ( ops )
		{
			case 0:	org.rotSteps = i; break;
			case 1: org.destSize.cx = i; break;
			case 2: org.destSize.cy = i; break;
			case 3: org.aaFactor = (BYTE) i; break;

			case 4: org.borderWidth = i / 100.0f; break;
			case 5: org.shadowDist = i / 100.0f; break;
			case 6: org.gradChaos = i / 100.0f; break;
		}
	}

	KillCS_Dump( &g_cPreview );

	g_cPreview = MakeCS_Dump( &org );

	if ( !g_cPreview ) MessageBox( gc_hWnd, TEXT( "Could not create 2ndHand!" ), 0, MB_ICONHAND );

	return g_cPreview;
}

WNDPROC c_eOld;

void InitCursorPreview()
{
	BYTE ops;

	for ( ops = 0; ops < CP_OPTIONS_NUMBER; ops ++ )
	{
		TCHAR buffy[ 4 ];
		wsprintf( buffy, TEXT( "%d" ), c_sOptions[ ( ops << 1 ) + 1 ] );

		o_hWnd[ ops ] = CreateWindow( TEXT( "EDIT" ), buffy , WS_CHILD | WS_VISIBLE | ES_NUMBER | ES_AUTOHSCROLL | ES_CENTER | ES_NOHIDESEL | ES_UPPERCASE, 0, 0, 0, 0, gc_hWnd, (HMENU) ops, 0, 0 );
	}

	b_hWnd = CreateWindow( TEXT( "BUTTON" ), TEXT( "Render" ), WS_DISABLED | WS_CHILD | WS_VISIBLE | BS_CENTER | BS_VCENTER | BS_TEXT | BS_DEFPUSHBUTTON, 0, 0, 0, 0, gc_hWnd, (HMENU) CP_OPTIONS_NUMBER, 0, 0 );
}

LRESULT CALLBACK c_pProc( HWND c_hWnd, UINT c_uMsg, WPARAM c_wParam, LPARAM c_lParam )
{
	if ( c_uMsg == WM_NCCREATE )
	{
		if ( gc_hWnd ) return 0;
		gc_hWnd = c_hWnd;

		InitCursorPreview();
	}

	else if ( c_uMsg == WM_GETMINMAXINFO )
	{
		if ( gc_hWnd )
		{
			MINMAXINFO *pMMI = (MINMAXINFO *) c_lParam;

			if ( pMMI->ptMinTrackSize.x < CP_PREVIEW_MINX ) pMMI->ptMinTrackSize.x = CP_PREVIEW_MINX;
			if ( pMMI->ptMinTrackSize.y < CP_PREVIEW_MINY ) pMMI->ptMinTrackSize.y = CP_PREVIEW_MINY;

			return 0;
		}
	}

	else if ( c_uMsg == WM_DESTROY )
	{
		gc_hWnd = 0;

		KillCS_Dump( &g_cPreview );
	}

	else if ( c_uMsg == WM_COMMAND )
	{
		if ( c_lParam == (LPARAM) b_hWnd )
		{
			if ( !Create2ndHand() )
			{
				InvalidateRect( c_hWnd, 0, 0 );
			}
		}
		else
		{
			BYTE ops;

			for ( ops = 0; ops < CP_OPTIONS_NUMBER; ops ++ )
			{
				if ( (HWND) c_lParam == o_hWnd[ ops ] )
				{
					InvalidateRect( c_hWnd, 0, 0 );
				}
			}
		}
	}

	else if ( c_uMsg == WM_SIZE )
	{
		WORD sX = LOWORD( c_lParam );
		WORD sY = HIWORD( c_lParam );

		BYTE ops;

		for ( ops = 0; ops < CP_OPTIONS_NUMBER; ops ++ )
		{
			SetWindowPos( o_hWnd[ ops ], HWND_TOP, CP_HOR_ALIGN( sX ), CP_VER_ALIGN( sY ), CP_HOR_ALIGN( sX ) >> 1, CP_BOX_HEIGHT, SWP_SHOWWINDOW );
		}

		SetWindowPos( b_hWnd, HWND_TOP, sX >> 3, sY - CP_BOX_HEIGHT * 3, sX * 3 >> 2, CP_BOX_HEIGHT * 2, SWP_SHOWWINDOW );

		InvalidateRect( c_hWnd, 0, 0 );
	}

	else if ( c_uMsg == WM_PAINT )
	{
		PAINTSTRUCT x_PS;
		RECT c_rSize;

		HDC hDC = BeginPaint( c_hWnd, &x_PS );
		GetClientRect( c_hWnd, &c_rSize );

		BitBlt( hDC, 0, 0, c_rSize.right, c_rSize.bottom, 0, 0, 0, BLACKNESS );
		{
			BYTE ops, sum = 0;

			SetBkColor( hDC, 0 );

			for ( ops = 0; ops < CP_OPTIONS_NUMBER; ops ++ )
			{
				CTRING c_sCur = c_sOptions[ ops << 1 ];

				int dX;
				RECT rect;

				if ( CheckEditBox( ops ) > 0 )
				{
					sum ++;
					SetTextColor( hDC, 0x00ffff );
				}
				else
				{
					SetTextColor( hDC, 0x0000ff );
				}

				rect.right = rect.left = 0;
				rect.top = rect.bottom = CP_VER_ALIGN( c_rSize.bottom ) + 1;

				DrawText( hDC, c_sCur, lstrlen( c_sCur ), &rect, DT_RIGHT | DT_SINGLELINE | DT_CALCRECT );

				dX = rect.right;

				rect.right = CP_HOR_ALIGN( c_rSize.right ) - 10;
				rect.left = rect.right - dX;

				DrawText( hDC, c_sCur, lstrlen( c_sCur ), &rect, DT_RIGHT | DT_SINGLELINE );

				if ( ops >= CP_OPTIONS_INTEGERS )
				{
					rect.left = ( CP_HOR_ALIGN( c_rSize.right ) * 3 >> 1 ) + 5;
					rect.right = rect.left + 20;
					DrawText( hDC, TEXT( "%" ), 1, &rect, DT_LEFT | DT_SINGLELINE );
				}
			}

			EnableWindow( b_hWnd, sum == CP_OPTIONS_NUMBER );
		}

		EndPaint( c_hWnd, &x_PS );
	}

	return DefWindowProc( c_hWnd, c_uMsg, c_wParam, c_lParam );
}

void CreateClient()
{
	SIZE screen, dims;

	CreateClassWindow( gc_sCN, gc_sWN, c_pProc, WS_CURSORPREVIEW, WX_CURSORPREVIEW );

	dims.cx = CP_PREVIEW_MINX;
	dims.cy = CP_PREVIEW_MINY;

	screen.cx = ( GetSystemMetrics( SM_CXSCREEN ) - dims.cx ) >> 1;
	screen.cy = ( GetSystemMetrics( SM_CYSCREEN ) - dims.cy ) >> 1;

	SetWindowPos( gc_hWnd, HWND_TOP, screen.cx, screen.cy, dims.cx, dims.cy, SWP_SHOWWINDOW );
}
