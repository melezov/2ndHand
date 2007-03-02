#include "CursorShop.h"

CTRING gs_sCN = TEXT( "2NDHAND_CURSOR_CLASS" );
CTRING gs_sWN = TEXT( "2ndHand Cursor" );

BOOL RaiseCS_Frame( CS_FRAME *f )
{
	if ( !f->hBM ) return ERROR_CURSORSHOP( 1 );

	f->hDC = CreateCompatibleDC( 0 );
	if ( !f->hDC ) return ERROR_CURSORSHOP( 2 );

	f->hGO = SelectObject( f->hDC, f->hBM );
	if ( !f->hGO ) return ERROR_CURSORSHOP( 3 );

	return ERROR_SUCCESS;
}

void KillCS_Frame( CS_FRAME *f )
{
	if ( f->hDC )
	{
		if ( f->hGO ) SelectObject( f->hDC, f->hGO );

		DeleteDC( f->hDC );
	}

	if ( f->hBM )
	{
		DeleteObject( f->hBM );
	}

	memset( f, 0, sizeof( CS_FRAME ) );
}

BOOL MakeCS_Frame( CS_FRAME *f, int f_iX, int f_iY )
{
	BOOL iRaise;
	memset( f, 0, sizeof( CS_FRAME ) );

	f->bih.biSize = sizeof( BITMAPINFOHEADER );
	f->bih.biPlanes = 1;
	f->bih.biBitCount = 32;
	f->bih.biWidth = f_iX;
	f->bih.biHeight = - f_iY;
	f->bih.biSizeImage = f_iX * f_iY << 2;

	f->hBM = CreateDIBSection( 0, (BITMAPINFO *) &f->bih, 0, &f->dPx, 0, 0 );

	iRaise = RaiseCS_Frame( f );

	if ( iRaise )
	{
		if ( iRaise > ERROR_CURSORSHOP( 1 ) ) KillCS_Frame( f );

		memset( f, 0, sizeof( CS_FRAME ) );
		return iRaise;
	}

	return ERROR_SUCCESS;
}

void KillCS_Factory( CS_FACTORY **pf )
{
	if ( *pf )
	{
		KillCS_Frame( &(*pf)->origin );
		KillCS_Frame( &(*pf)->stretch );
		KillCS_Frame( &(*pf)->grid );

		VirtualFree( *pf, 0, MEM_FREE );
		*pf = 0;
	}
}

void KillCS_Dump( CS_DUMP **pd )
{
	if ( *pd )
	{
		KillCS_Frame( &(*pd)->dump );
		KillCS_Frame( &(*pd)->thumb );

		if ( (*pd)->body ) DestroyWindow( (*pd)->body );

		VirtualFree( *pd, 0, MEM_FREE );
		*pd = 0;
	}
}

BOOL MakeCS_Stretch( CS_FACTORY *f )
{
	int x, y;

	CS_FRAME *o = &f->origin;
	CS_FRAME *n = &f->stretch;

	if ( !f ) return ERROR_CURSORSHOP( 0 );

	memset( &f->palette, 0, sizeof( CS_COLORINFO ) * CS_MAX_COLORS );

	for ( y = 0; y < o _Y; y ++ )
	for ( x = 0; x < o _X; x ++ )
	{
		COLORREF t_dCol = _PX( o, x, y );

		if ( t_dCol )
		{
			CS_COLORINFO *t_dCur;

			for ( t_dCur = f->palette; ; t_dCur ++ )
			{
				if ( ! t_dCur->rgb || ( t_dCur->rgb == t_dCol ) )
				{
					t_dCur->rgb = t_dCol;
					t_dCur->cnt ++;
					t_dCur->pnt.x += x + 0.5f;
					t_dCur->pnt.y += y + 0.5f;

					break;
				}
			}
		}
	}

	#define pal( q ) f->palette[ q ]

	for ( y =     0; y < CS_MAX_COLORS - 1; y ++ ) if   ( pal( y ).rgb != CS_WHITE )
	for ( x = y + 1; x < CS_MAX_COLORS;     x ++ ) if ( ( pal( x ).rgb == CS_WHITE ) || ( pal( x ).cnt > pal( y ).cnt ) )
	{
		CS_COLORINFO tempSwap;

		memcpy( &tempSwap, &pal( y ), sizeof( CS_COLORINFO ) );
		memcpy( &pal( y ), &pal( x ), sizeof( CS_COLORINFO ) );
		memcpy( &pal( x ), &tempSwap, sizeof( CS_COLORINFO ) );
	}

	for ( x = 0; x < CS_MAX_COLORS; x ++ )
	{
		pal( x ).pnt.x = ( pal( x ).pnt.x / pal( x ).cnt ) / o _X;
		pal( x ).pnt.y = ( pal( x ).pnt.y / pal( x ).cnt ) / o _Y;
	}

	x = MakeCS_Frame( n, f->header.destSize.cx * f->header.aaFactor, f->header.destSize.cy * f->header.aaFactor );
	if ( x ) return x;

	srand( f->header.randSeed );

	for ( y = 0; y < o _Y; y ++ )
	for ( x = 0; x < o _X; x ++ )
	{
		COLORREF t_dCol = _PX( o, x, y );

		if ( t_dCol == CS_BLACK ) continue;

		if ( t_dCol != CS_WHITE )
		{
			const float fX = (float) n _X / o _X;
			const float fY = (float) n _Y / o _Y;

			int a, b;

			for ( b = 0; b < fY; b ++ )
			for ( a = 0; a < fX; a ++ )
			{
				int c = (int) ( x * fX + a );
				int d = (int) ( y * fY + b );

				BYTE rgb;
				COLORREF *t_dCur = &_PX( n, c, d );

				float perc;
				{
					float deltas[ 2 ];

					for ( rgb = 0; rgb < 2; rgb ++ )
					{
						float dX = c - pal( rgb ).pnt.x * n _X;
						float dY = d - pal( rgb ).pnt.y * n _Y;

						deltas[ 1 - rgb ] = dX * dX + dY * dY;
					}

					perc = deltas[ 1 ] / ( deltas[ 0 ] + deltas[ 1 ] ) + f->header.gradChaos * ( 0.5f - (float) rand() / RAND_MAX );
				}

				*t_dCur = CS_ALPHA_MASK;

				for ( rgb = 0; rgb < 3; rgb ++ )
				{
					float grad =	(float) ( (BYTE *) &pal( 1 ).rgb )[ rgb ] * perc +
									(float) ( (BYTE *) &pal( 2 ).rgb )[ rgb ] * ( 1 - perc );

					if ( grad < 0.0f ) grad = 0.0f; else if ( grad > 255.0f ) grad = 255.0f;

					( (BYTE *) t_dCur )[ rgb ] = (BYTE) grad;
				}
			}
		}
	}

	return ERROR_SUCCESS;
}

long ftol( float z )
{
	return (long) ( z + 0.5f );
}

void pntf2pntl( POINT *dst, POINTFLOAT *src )
{
	dst->x += ftol( src->x );
	dst->y += ftol( src->y );
}

BOOL MakeCS_Rotation( CS_FACTORY *f, WORD index, BOOL runMode )
{
	BYTE aaF = f->header.aaFactor;
	SIZE q_sz;

	CS_FRAME r;
	CS_FRAME *n = &f->stretch;
	CS_FRAME *g = &f->grid;

	BOOL iMake;
	float r_fAngle = (float) M_PI * 2 * index / f->header.rotSteps;

	POINTFLOAT r_fX, r_fY;
	POINT r_pPlg[ 4 ] = { 0 };
	RECT r_rBounds[ 3 ];

	int i, k, x, y, a, b;

	r_fX.x = n _X * sinf( - r_fAngle + (float) M_PI_2 );
	r_fX.y = n _X * sinf(   r_fAngle );
	r_fY.x = n _Y * sinf( - r_fAngle );
	r_fY.y = n _Y * sinf(   r_fAngle + (float) M_PI_2 );

	for ( i = 0; i < 4; i ++ )
	{
		RECT *cRct = i < 3 ? &r_rBounds[ i ] : &f->dims[ index ].bounds;

		cRct->left = cRct->top = LONG_MAX;
		cRct->right = cRct->bottom = LONG_MIN;
	};

	i = (int) ( f->header.shadowDist * f->header.aaFactor );
	k = (int) ( f->header.borderWidth * f->header.aaFactor );

	for ( y = 0; y < 2; y ++ )
	for ( x = 0; x < 4; x ++ )
	{
		POINT *cPnt = &r_pPlg[ x ];

		if ( !y )
		{
			if ( x & 1 ) pntf2pntl( cPnt, &r_fX );
			if ( x > 1 ) pntf2pntl( cPnt, &r_fY );
		}
		else
		{
			cPnt->x -= r_rBounds[ 0 ].left - k;
			cPnt->y -= r_rBounds[ 0 ].top  - k;
		}

		TestRect( &r_rBounds[ y ], cPnt );
	}

	q_sz.cx = r_rBounds[ 1 ].right + i + k + 1;
	q_sz.cy = r_rBounds[ 1 ].bottom + i + k + 1;

	if ( runMode == CS_PARAMETAR_HAX )
	{
		if ( f->maxFrame.cx < q_sz.cx / aaF ) f->maxFrame.cx = q_sz.cx / aaF;
		if ( f->maxFrame.cy < q_sz.cy / aaF ) f->maxFrame.cy = q_sz.cy / aaF;

		return ERROR_SUCCESS;
	}

	iMake = MakeCS_Frame( &r, q_sz.cx, q_sz.cy );
	if ( iMake ) return iMake;

	WaitForSingleObject( f->plgMutex, INFINITE );

	if( !PlgBlt( r.hDC, r_pPlg, n->hDC, 0, 0, n _X, n _Y, 0, 0, 0 ) )
	{
		KillCS_Frame( &r );
		return ERROR_CURSORSHOP( 9 );
	}

	ReleaseMutex( f->plgMutex );

	for ( y = r_rBounds[ 1 ].top; y <= r_rBounds[ 1 ].bottom; y ++ )
	for ( x = r_rBounds[ 1 ].left; x <= r_rBounds[ 1 ].right; x ++ )
	{
		COLORREF *r_dSeek = &_PX( &r, x, y );

		if ( *r_dSeek & CS_WHITE )
		{
			COLORREF *c_dSeek = &r_dSeek[ i * ( 1 + (&r) _X ) ];

			if ( ! *c_dSeek )
			{
				*c_dSeek = CS_SHADOW_DARK;
			}

			for ( b = -k; b <= k; b ++ )
			for ( a = -k; a <= k; a ++ )
			{
				if ( a * a + b * b < k * k )
				{
					COLORREF *c_dSeek = &r_dSeek[ a + b * (&r) _X ];

					if ( ! ( *c_dSeek & CS_WHITE ) )
					{
						POINT c_pHit = { x + a, y + b };

						COLORREF *s_dSeek = &c_dSeek[ i * ( 1 + (&r) _X ) ];
						*c_dSeek = CS_ALPHA_MASK;

						TestRect( &r_rBounds[ 2 ], &c_pHit );

						if ( ! *s_dSeek )
						{
							*s_dSeek = CS_SHADOW_LIGHT;

							c_pHit.x += i;
							c_pHit.y += i;
							TestRect( &r_rBounds[ 2 ], &c_pHit );
						}
					}
				}
			}
		}
	}

	q_sz.cx = ( r_rBounds[ 2 ].right - r_rBounds[ 2 ].left + 1 ) / aaF;
	q_sz.cy = ( r_rBounds[ 2 ].bottom - r_rBounds[ 2 ].top + 1 ) / aaF;

	WaitForSingleObject( f->plgMutex, INFINITE );

	if ( f->maxFrame.cx < q_sz.cx ) f->maxFrame.cx = q_sz.cx;
	if ( f->maxFrame.cy < q_sz.cy ) f->maxFrame.cy = q_sz.cy;

	ReleaseMutex( f->plgMutex );

	for ( y = 0; y < q_sz.cy; y ++ )
	for ( x = 0; x < q_sz.cx; x ++ )
	{
		DWORD c, argb[ 4 ] = { 0 };

		for ( b = 0; b < aaF; b ++ )
		for ( a = 0; a < aaF; a ++ )
		{
			COLORREF *c_dSeek = &_PX( &r, x * aaF + a + r_rBounds[ 2 ].left,
								 		  y * aaF + b + r_rBounds[ 2 ].top );
			if ( *c_dSeek )
			{
				for ( c = 0; c < 4; c ++ )
				{
					argb[ c ] += ( (BYTE *) c_dSeek )[ c ];
				}
			}
		}

		if ( argb[ 3 ] )
		{
			POINT qHit = { x, y };

			for ( c = 0; c < 4; c ++ )
			{
				( (BYTE *) &_PX( g, x, y + f->maxFrame.cy * index ) )[ c ] = (BYTE) ( argb[ c ] / ( aaF * aaF ) );
			}

			TestRect( &f->dims[ index ].bounds, &qHit );
		}
	}

	KillCS_Frame( &r );

	f->dims[ index ].hotSpot.x = ( r_pPlg[ 0 ].x - r_rBounds[ 2 ].left + ftol( pal( 0 ).pnt.x * r_fX.x ) + ftol( pal( 0 ).pnt.y * r_fY.x ) ) / aaF;
	f->dims[ index ].hotSpot.y = ( r_pPlg[ 0 ].y - r_rBounds[ 2 ].top  + ftol( pal( 0 ).pnt.x * r_fX.y ) + ftol( pal( 0 ).pnt.y * r_fY.y ) ) / aaF;

	return ERROR_SUCCESS;
}

BOOL MakeCS_Grid( CS_FACTORY *f )
{
	BOOL iMake;
	WORD rotSteps;

	BOOL runMode = (BYTE) f & CS_PARAMETAR_HAX;

	(BYTE *) f -= runMode;

	for ( rotSteps = runMode; rotSteps < f->header.rotSteps; rotSteps += 2 )
	{
		iMake = MakeCS_Rotation( f, rotSteps, runMode );
		if ( iMake ) return iMake;
	}

	return ERROR_SUCCESS;
}

BOOL TransferCS_Grid( CS_FACTORY *f, CS_DUMP *d )
{
	int iMake;
	WORD rotSteps;

	long dX = LONG_MIN;
	long dY = LONG_MIN;

	for ( rotSteps = 0; rotSteps < d->header.rotSteps; rotSteps ++ )
	{
		long rX = f->dims[ rotSteps ].bounds.right - f->dims[ rotSteps ].bounds.left + 1;
		long rY = f->dims[ rotSteps ].bounds.bottom - f->dims[ rotSteps ].bounds.top + 1;

		if ( dX < rX ) dX = rX;
		if ( dY < rY ) dY = rY;
	}

	d->minFrame.cx = dX;
	d->minFrame.cy = dY;

	iMake = MakeCS_Frame( &d->dump, dX, dY * d->header.rotSteps );
	if ( iMake ) return iMake;

	for ( rotSteps = 0; rotSteps < d->header.rotSteps; rotSteps ++ )
	{
		long sX = f->dims[ rotSteps ].bounds.left;
		long sY = f->dims[ rotSteps ].bounds.top;

		long rX = f->dims[ rotSteps ].bounds.right  - sX + 1;
		long rY = f->dims[ rotSteps ].bounds.bottom - sY + 1;

		long lX = ( dX - rX ) >> 1;
		long lY = ( dY - rY ) >> 1;

		BitBlt( d->dump.hDC, lX, lY + dY * rotSteps, rX, rY, f->grid.hDC, sX, sY + f->maxFrame.cy * rotSteps, SRCCOPY );

		d->factory.dims[ rotSteps ].hotSpot.x = f->dims[ rotSteps ].hotSpot.x + lX;
		d->factory.dims[ rotSteps ].hotSpot.y = f->dims[ rotSteps ].hotSpot.y + lY;
	}

	return ERROR_SUCCESS;
}

void MoveCS_Dump( CS_DUMP *d, LPARAM s_lParam )
{
	BLENDFUNCTION blenda = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
	POINT pNew = { (short) LOWORD( s_lParam ), (short) HIWORD( s_lParam ) };

	WORD rot = (WORD) ( ( ( ( LOWORD( s_lParam ) / 4 ) % d->header.rotSteps ) + d->header.rotSteps ) % d->header.rotSteps );

	POINT t_pSrc = { 0, d->minFrame.cy * rot };
	POINT t_pDst = { pNew.x - d->factory.dims[ rot ].hotSpot.x, pNew.y - d->factory.dims[ rot ].hotSpot.y };
	SIZE  t_pSiz = { d->minFrame.cx, d->minFrame.cy };

	UpdateLayeredWindow( d->body, 0, &t_pDst, &t_pSiz, d->dump.hDC, &t_pSrc, 0, &blenda, ULW_ALPHA );
//	SetWindowPos( d->body, HWND_TOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE );
}

LRESULT CALLBACK s_pProc( HWND s_hWnd, UINT s_uMsg, WPARAM s_wParam, LPARAM s_lParam )
{
	if ( s_uMsg == WM_CLOSE ) return 0;

	return DefWindowProc( s_hWnd, s_uMsg, s_wParam, s_lParam );
}

HWND WakeCS_Dump( CS_DUMP *d )
{
	return d->body = CreateClassWindow( gs_sCN, gs_sWN, s_pProc, WS_CURSORSHOP, WX_CURSORSHOP );
}

CS_DUMP *MakeCS_Dump( CS_HEADER *h )
{
	CS_DUMP *d;
	CS_FACTORY *f;
	CS_FRAME o;

	int iMake, kMake;

	HANDLE thread;

	o.hBM = LoadBitmap( ge_hInst, MAKEINTRESOURCE( h->cursorId ) );

	if ( !GetObject( o.hBM, sizeof( BITMAP ), &o.bih ) )	return 0;
	o.bih.biHeight *= -1;

	if ( RaiseCS_Frame( &o ) != ERROR_SUCCESS )				return 0;

	f = (CS_FACTORY *) VirtualAlloc( 0, sizeof( CS_FACTORY ) + sizeof( CS_DIMINFO ) * h->rotSteps, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE );
	if ( !f ) goto OriginError;

	if ( MakeCS_Frame( &f->origin, (&o) _X, (&o) _Y ) ) goto OriginError;

	if ( !BitBlt( f->origin.hDC, 0, 0, (&o) _X, (&o) _Y, o.hDC, 0, 0, SRCCOPY ) )
	{
		OriginError:	KillCS_Frame( &o );
		FactoryError:	KillCS_Factory( &f );

		return 0;
	}

	KillCS_Frame( &o );

	memcpy( &f->header, h, sizeof( CS_HEADER ) );
	if ( MakeCS_Stretch( f ) ) goto FactoryError;

	f->maxFrame.cx = f->maxFrame.cy = LONG_MIN;

	if ( MakeCS_Grid( (CS_FACTORY *) ( (BYTE *) f + CS_PARAMETAR_HAX ) ) ) goto FactoryError;
	if ( MakeCS_Frame( &f->grid, f->maxFrame.cx, f->maxFrame.cy * f->header.rotSteps ) ) goto FactoryError;

	f->plgMutex = CreateMutex( 0, 0, 0 );

	thread = CreateThread( 0, 0, (LPTHREAD_START_ROUTINE) MakeCS_Grid, (BYTE *) f + 1, 0, 0 );

	iMake = MakeCS_Grid( f );
	WaitForSingleObject( thread, INFINITE );
	CloseHandle( f->plgMutex );

	GetExitCodeThread( thread, &kMake );
	CloseHandle( thread );

	if ( iMake || kMake ) goto FactoryError;

	d = (CS_DUMP *) VirtualAlloc( 0, sizeof( CS_DUMP ) + sizeof( CS_DIMINFO ) * h->rotSteps, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE );
	if ( !d ) goto FactoryError;

	memcpy( &d->header, h, sizeof( CS_HEADER ) );

	TransferCS_Grid( f, d );
	KillCS_Factory( &f );

	if ( !WakeCS_Dump( d ) )
	{
		KillCS_Dump( &d );
	}

	SaveCS_Frame( &d->dump, "c:\\Slike.bmp" );

	return d;
}
