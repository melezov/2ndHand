#include "CursorPreview.h"

#define CP_OPTIONS_NUMBER 8

STRING p_sOptions[ CP_OPTIONS_NUMBER << 1 ] =
{
	TEXT( "Rotation frames:" ),			(STRING) 256,
	TEXT( "Preferred Width:" ),			(STRING)  80,
	TEXT( "Preferred Height:" ),		(STRING)  60,
	TEXT( "Quality (AA factor):" ),		(STRING)   3,
	TEXT( "Plasma seed:" ),				(STRING)   0,
	TEXT( "Border thickness:\n[%]" ),	(STRING) 250,
	TEXT( "Shadow distance:\n[%]" ),	(STRING) 500,
	TEXT( "Plasma chaos:\n[%]" ),		(STRING)  60
};

typedef struct CP_I
{
	HWND		hWnd;
	HWND		cWnd;

	HWND		tWnd;
	HWND		eWnd;
	HWND		sWnd[ CP_OPTIONS_NUMBER ];
	HWND		oWnd[ CP_OPTIONS_NUMBER ];
	HWND		bWnd;

	CS_HEADER	head;
	CS_FACTORY	*cpF;

	struct CP_I	*next;
}
CP_INFO;

HANDLE headMutex = 0;
CP_INFO *head = 0;

LRESULT CALLBACK c_pProc( HWND c_hWnd, UINT c_uMsg, WPARAM c_wParam, LPARAM c_lParam )
{
	return DefWindowProc( c_hWnd, c_uMsg, c_wParam, c_lParam );
}

void RegisterCP_Info( CP_INFO *n )
{
	CS_FACTORY *f = MakeCS_Factory( &n->head );

	BOOL erase = n->cpF != 0;

	if ( !f )
	{
		MessageBox( n->hWnd, "Error creating cursor", 0, MB_ICONHAND );
		return;
	}

	if ( n->cpF )
	{
		KillCS_Factory( &n->cpF );
	}

	n->cpF = f;

	RenderCS_Factory( n->cpF );

	if ( !n->cWnd ) n->cWnd = CreateClassWindow( gc_sCN, gc_sWN, c_pProc, WS_2NDHANDCURSOR, WX_2NDHANDCURSOR );
	if ( !headMutex ) headMutex = CreateMutex( 0, 0, 0 );

	WaitForSingleObject( headMutex, INFINITE );

	if ( !erase )
	{
		n->next = head;
		head = n;
	}

	ReleaseMutex( headMutex );
}

void MoveLaddy( WPARAM m_wParam, LPARAM m_lParam )
{
	CP_INFO *seek = head;
	if ( !seek ) return;

	WaitForSingleObject( headMutex, INFINITE );
	for ( ; seek; seek = seek->next )
	{
		SendMessage( seek->hWnd, WM_RATHOOK, m_wParam, m_lParam );
	}
	ReleaseMutex( headMutex );
}

CTRING defaultCursor = "..\\Cursor Bitmaps\\0x10 - Pointer.bmp";

BOOL ReadCS_Header( CP_INFO *p )
{
	STRING buffy = p->head.resId;
	HANDLE testFile;
	BYTE ops, errorCnt = 0;

	memset( &p->head, 0, sizeof( CS_HEADER ) );

	for ( ops = 0; ops < CP_OPTIONS_NUMBER; ops ++ )
	{
		int parse;

		GetWindowText( p->oWnd[ ops ], buffy, MAX_PATH );
		parse = atoi( buffy );

		EnableWindow( p->sWnd[ ops ], parse > 0 );

		if ( parse > 0 )
		{
			switch ( ops )
			{
				case 0:	p->head.rotSteps = parse; break;
				case 1: p->head.destSize.cx = parse; break;
				case 2: p->head.destSize.cy = parse; break;
				case 3: p->head.aaFactor = (BYTE) parse; break;
				case 4: p->head.randSeed = parse; break;
				case 5: p->head.borderWidth = parse / 100.0f; break;
				case 6: p->head.shadowDist = parse / 100.0f; break;
				case 7: p->head.gradChaos = parse / 100.0f; break;
			}
		}
		else errorCnt ++;
	}

	GetWindowText( p->eWnd, p->head.resId, MAX_PATH );
	testFile = CreateFile( p->head.resId, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 );

	EnableWindow( p->tWnd, testFile != INVALID_HANDLE_VALUE );
	if ( testFile == INVALID_HANDLE_VALUE ) errorCnt ++;
	CloseHandle( testFile );

	EnableWindow( p->bWnd, !errorCnt );

	return errorCnt ? ERROR_CURSORPREVIEW( 20 + errorCnt ) : ERROR_SUCCESS;
}

BOOL InitCursorPreview( HWND p_hWnd )
{
	BYTE ops;

	CP_INFO *p = (CP_INFO *) VirtualAlloc( 0, sizeof( CP_INFO ), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE );
	if ( !p ) return ERROR_CURSORPREVIEW( 1 );

	SetWindowLongPtr( p_hWnd, GWLP_USERDATA, (long) p );

	p->hWnd = p_hWnd;

	srand( GetTickCount() );
	p_sOptions[ 9 ] = (STRING) rand();

	p->tWnd = CreateWindow( TEXT( "STATIC" ), TEXT( "Path to Cursor Bitmap:" ), WS_CHILD | WS_VISIBLE | WS_DISABLED | SS_CENTER, 0, 0, 0, 0, p_hWnd, (HMENU) 0x7F, 0, 0 );
	p->eWnd = CreateWindow( TEXT( "EDIT" ), defaultCursor, WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_CENTER | ES_NOHIDESEL | ES_MULTILINE, 0, 0, 0, 0, p_hWnd, (HMENU) 0x7E, 0, 0 );
	p->bWnd = CreateWindow( TEXT( "BUTTON" ), TEXT( "Render" ), WS_DISABLED | WS_CHILD | WS_VISIBLE | BS_CENTER | BS_VCENTER | BS_TEXT | BS_DEFPUSHBUTTON, 0, 0, 0, 0, p_hWnd, (HMENU) 0x7D, 0, 0 );

	for ( ops = 0; ops < CP_OPTIONS_NUMBER; ops ++ )
	{
		STRING *pCur = &p_sOptions[ ops << 1 ];

		TCHAR buffy[ MAX_PATH ];
		wsprintf( buffy, TEXT( "%d" ), pCur[ 1 ] );

		p->oWnd[ ops ] = CreateWindow( TEXT( "EDIT" ), buffy , WS_CHILD | WS_VISIBLE | ES_NUMBER | ES_AUTOHSCROLL | ES_CENTER | ES_NOHIDESEL | ES_UPPERCASE, 0, 0, 0, 0, p_hWnd, (HMENU) ( ( ops << 1 ) + 1 ), 0, 0 );
		p->sWnd[ ops ] = CreateWindow( TEXT( "STATIC" ), pCur[ 0 ], WS_CHILD | WS_VISIBLE | WS_DISABLED | SS_CENTER, 0, 0, 0, 0, p_hWnd, (HMENU) ( ops << 1 ), 0, 0 );
	}

	ReadCS_Header( p );

	return ERROR_SUCCESS;
}

void DrawCursor( CP_INFO *p, WPARAM p_wParam, LPARAM p_lParam )
{
	CS_FACTORY *f = p->cpF;
	CS_FRAME *d;

	WORD rot = (WORD) ( LOWORD( p_lParam ) % f->head.rotSteps );
	while ( rot < 0 ) rot += f->head.rotSteps;

	d = &f->rots[ rot ];

	if ( d->flag == CS_FLAG_RENDERED )
	{
		BLENDFUNCTION blenda = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
		POINT pNew = { (short) LOWORD( p_lParam ), (short) HIWORD( p_lParam ) };

		POINT t_pSrc = { 0, 0 };
		POINT t_pDst = { pNew.x - d->fpi.pCenter.x, pNew.y - d->fpi.pCenter.y };
		SIZE  t_pSiz = { d _X, d _Y };

		UpdateLayeredWindow( p->cWnd, 0, &t_pDst, &t_pSiz, d->dci.hDC, &t_pSrc, 0, &blenda, ULW_ALPHA );
	}
	else
	{
		volatile BYTE *bFlag = &f->rots[ rot ].flag;

		WaitForSingleObject( f->mutex, INFINITE );

		if ( *bFlag == CS_FLAG_IDLE ) *bFlag = CS_FLAG_PRIORITY;

		ReleaseMutex( f->mutex );
	}
}

LRESULT CALLBACK p_pProc( HWND p_hWnd, UINT p_uMsg, WPARAM p_wParam, LPARAM p_lParam )
{
	CP_INFO *p = (CP_INFO *) GetWindowLongPtr( p_hWnd, GWLP_USERDATA );

	if ( !p )
	{
		if ( InitCursorPreview( p_hWnd ) ) return 0;
	}
	else
	{
		if ( p_uMsg == WM_NCDESTROY )
		{
			KillCS_Factory( &p->cpF );
			DestroyWindow( p->cWnd );

			p->hWnd = p->cWnd = 0;
		}

		else if ( p_uMsg == WM_RATHOOK )
		{
			DrawCursor( p, p_wParam, p_lParam );
		}

		else if ( p_uMsg == WM_COMMAND )
		{
			if ( p_lParam == (LPARAM) p->bWnd )
			{
				if ( !ReadCS_Header( p ) )
				{
					RegisterCP_Info( p );
				}
			}
			else if ( p_lParam == (LPARAM) p->eWnd )
			{
				ReadCS_Header( p );
			}
			else
			{
				BYTE ops;

				for ( ops = 0; ops < CP_OPTIONS_NUMBER; ops ++ )
				{
					if ( p_lParam == (LPARAM) p->oWnd[ ops ] )
					{
						ReadCS_Header( p );
					}
				}
			}

			return 0;
		}

		else if ( p_uMsg == WM_SIZE )
		{
			WORD sX = LOWORD( p_lParam );
			WORD sY = HIWORD( p_lParam );

			BYTE ups, ops, width = sY / ( CP_OPTIONS_NUMBER + 3 );

			SetWindowPos( p->tWnd, HWND_TOP, 0,     0, sX, width, SWP_SHOWWINDOW );
			SetWindowPos( p->eWnd, HWND_TOP, 0, width, sX, width, SWP_SHOWWINDOW );

			InvalidateRect( p->tWnd, 0, 0 );

			for ( ops = 0; ops < CP_OPTIONS_NUMBER; ops ++ )
			{
				ups = ops + 2;

				SetWindowPos( p->sWnd[ ops ], HWND_TOP,       0, width * ups, sX >> 1, width, SWP_SHOWWINDOW );
				SetWindowPos( p->oWnd[ ops ], HWND_TOP, sX >> 1, width * ups, sX >> 1, width, SWP_SHOWWINDOW );

				InvalidateRect( p->sWnd[ ops ], 0, 0 );
			}

			SetWindowPos( p->bWnd, HWND_TOP, 0, width * ++ ups, sX, sY - width * ups, SWP_SHOWWINDOW );
		}
	}

	return DefWindowProc( p_hWnd, p_uMsg, p_wParam, p_lParam );
}

BOOL CreateClient()
{
	SIZE screen, dims;

	HWND p_hWnd = CreateClassWindow( gp_sCN, gp_sWN, p_pProc, WS_CURSORPREVIEW, WX_CURSORPREVIEW );
	if ( !p_hWnd ) return ERROR_CURSORPREVIEW( 0 );

	screen.cx = GetSystemMetrics( SM_CXSCREEN );
	screen.cy = GetSystemMetrics( SM_CYSCREEN );

	dims.cx = screen.cx >> 2;
	dims.cy = screen.cy >> 1;

	screen.cx = ( screen.cx - dims.cx ) >> 1;
	screen.cy = ( screen.cy - dims.cy ) >> 1;

	SetWindowPos( p_hWnd, HWND_TOP, screen.cx, screen.cy, dims.cx, dims.cy, SWP_SHOWWINDOW );

	return ERROR_SUCCESS;
}
