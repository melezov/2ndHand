#include "CursorFactory.h"
#include "CursorFactoryLook.h"

HINSTANCE c_hInst;

CF_WS *MakeCF_WS( HBITMAP n_hBM )
{
	CF_WS *n;

	if ( !n_hBM ) return 0;

	if ( ( n = (CF_WS *) VirtualAlloc( 0, sizeof( CF_WS ), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE ) ) != 0 )
	{
		GetObject( n->hBM = n_hBM, sizeof( BITMAP ), &n->bmp );
		n->hDC = CreateCompatibleDC( 0 );
		n->hGO = SelectObject( n->hDC, n->hBM );
	}

	return n;
}

void KillCF_WS( CF_WS *k )
{
	SelectObject( k->hDC, k->hGO );
	DeleteDC( k->hDC );

	DeleteObject( k->hBM );
	VirtualFree( k, 0, MEM_RELEASE );
}

HBITMAP MakeBM( int s_iX, int s_iY )
{
	LPVOID t_dPx;

	BITMAPINFO t_xBMI = { 0 };
	{
		t_xBMI.bmiHeader.biSize = sizeof( BITMAPINFOHEADER );
		t_xBMI.bmiHeader.biWidth = s_iX;
		t_xBMI.bmiHeader.biHeight = - s_iY;
		t_xBMI.bmiHeader.biPlanes = 1;
		t_xBMI.bmiHeader.biBitCount = 32;
		t_xBMI.bmiHeader.biCompression = BI_RGB;
		t_xBMI.bmiHeader.biSizeImage = s_iX * s_iY << 2;
	}

	return CreateDIBSection( 0, &t_xBMI, DIB_RGB_COLORS, &t_dPx, 0, 0 );
}

CF_WS *LoadCF_WS( BYTE resID )
{
	CF_CI cI[ 1 << CF_MAX_BIT_DEPTH ] = { 0 };
	int x, y;

	CF_WS *n;
	{
		CF_WS *o = MakeCF_WS( LoadBitmap( c_hInst, MAKEINTRESOURCE( resID ) ) );

		if ( !o ) return 0;

		n = MakeCF_WS( MakeBM( CF_CURSOR_XSIZE * CF_ENLARGE_FACTOR, CF_CURSOR_YSIZE * CF_ENLARGE_FACTOR ) );

		if ( !n )
		{
			KillCF_WS( o );
			return 0;
		}

		for ( y = 0; y < o _Y; y ++ )
		for ( x = 0; x < o _X; x ++ )
		{
			int index;

			COLORREF t_dCol = GetPixel( o->hDC, x, y );

			if ( t_dCol == CF_BLACK ) continue;

			if ( t_dCol != CF_WHITE )
			{
				float fX = (float) n _X / o _X;
				float fY = (float) n _Y / o _Y;

				int a, b;

				for ( b = 0; b < fY; b ++ )
				for ( a = 0; a < fX; a ++ )
				{
					_PX( n, (int) ( x * fX + a ), (int) ( y * fY + b ) ) = 0xFFFFFFFF;
				}
			}

			for ( index = 0; ; index ++ )
			{
				CF_CI *t_dCur = &cI[ index ];

				if ( ! t_dCur->rgb || ( t_dCur->rgb == t_dCol ) )
				{
					t_dCur->rgb = t_dCol;
					t_dCur->cnt ++;
					t_dCur->pnt.x += x;
					t_dCur->pnt.y += y;

					break;
				}
			}
		}

		for ( y = 0; y < ( 1 << CF_MAX_BIT_DEPTH ) - 1; y ++ )
		for ( x = y; x < ( 1 << CF_MAX_BIT_DEPTH );     x ++ )
		{
			CF_CI *t_dCurCY = &cI[ y ];
			CF_CI *t_dCurCX = &cI[ x ];

			if ( ( t_dCurCX->rgb == CF_WHITE ) || ( t_dCurCY->cnt < t_dCurCX->cnt ) )
			{
				CF_CI t_dTemp;

				memcpy( &t_dTemp, t_dCurCY, sizeof( CF_CI ) );
				memcpy( t_dCurCY, t_dCurCX, sizeof( CF_CI ) );
				memcpy( t_dCurCX, &t_dTemp, sizeof( CF_CI ) );
			}
		}

		n->pRot.x = ( (float) cI[ 0 ].pnt.x / cI[ 0 ].cnt + 0.5f ) / o _X;
		n->pRot.y = ( (float) cI[ 0 ].pnt.y / cI[ 0 ].cnt + 0.5f ) / o _Y;

		KillCF_WS( o );
	}

	for ( x = 0; x < n _X; x ++ )
	{
		float perc = (float) x / ( n _X - 1 );

		for ( y = 0; y < n _Y; y ++ )
		{
			BYTE rgb;
			COLORREF *n_dSeek = &_PX( n, x, y );

			if ( ! *n_dSeek ) continue;

			for ( rgb = 0; rgb < 3; rgb ++ )
			{
				float grad = (float) ( (BYTE *) &cI[ 2 ].rgb )[ 2 - rgb ] * perc +
							 (float) ( (BYTE *) &cI[ 1 ].rgb )[ 2 - rgb ] * ( 1 - perc );

				if ( grad < 0.0f ) grad = 0.0f; else if ( grad > 255.0f ) grad = 255.0f;

				( (BYTE *) n_dSeek )[ rgb ] = (BYTE) grad;
			}
		}
	}

	return n;
}

void TestRect( RECT *s_rBounds, POINT *s_pHit )
{
	if ( s_rBounds->left   > s_pHit->x ) s_rBounds->left   = s_pHit->x;
	if ( s_rBounds->top    > s_pHit->y ) s_rBounds->top    = s_pHit->y;
	if ( s_rBounds->right  < s_pHit->x ) s_rBounds->right  = s_pHit->x;
	if ( s_rBounds->bottom < s_pHit->y ) s_rBounds->bottom = s_pHit->y;
}

CF_WS *DeriveCF_WS( CF_WS *n, float r_fAngle )
{
	SIZE q_sz;
	CF_WS *r, *q;

	float r_fXC = n _X * sinf( - r_fAngle + (float) M_PI_2 );
	float r_fXS = n _X * sinf(   r_fAngle );
	float r_fYC = n _Y * sinf( - r_fAngle );
	float r_fYS = n _Y * sinf(   r_fAngle + (float) M_PI_2 );

	RECT r_rBounds[ 4 ];

	#define DELTA (int) ( CF_BORDER_WIDTH * CF_ENLARGE_FACTOR )

	POINT r_pPlg[ 4 ] = { 0 };

	int i, k, x, y, a, b;

	for ( i = 0; i < 4; i ++ )
	{
		RECT *cRct = &r_rBounds[ i ];

		cRct->left = cRct->top = LONG_MAX;
		cRct->right = cRct->bottom = LONG_MIN;
	};

	for ( k = 0; k < 2; k ++ )
	for ( i = 0; i < 4; i ++ )
	{
		POINT *cPnt = &r_pPlg[ i ];

		if ( !k )
		{
			if ( i & 1 )
			{
				cPnt->x += (int) r_fXC;
				cPnt->y += (int) r_fXS;
			}

			if ( i > 1 )
			{
				cPnt->x += (int) r_fYC;
				cPnt->y += (int) r_fYS;
			}
		}
		else
		{
			cPnt->x -= r_rBounds[ 0 ].left - DELTA;
			cPnt->y -= r_rBounds[ 0 ].top  - DELTA;
		}

		TestRect( &r_rBounds[ k ], cPnt );
	}

	#define EPSILON ( DELTA + CF_SHADOW_DIST * CF_ENLARGE_FACTOR )

	r = MakeCF_WS( MakeBM( r_rBounds[ 1 ].right + EPSILON, r_rBounds[ 1 ].bottom + EPSILON ) );
	PlgBlt( r->hDC, r_pPlg, n->hDC, 0, 0, n _X, n _Y, 0, 0, 0 );

	for ( y = r_rBounds[ 1 ].top; y <= r_rBounds[ 1 ].bottom; y ++ )
	for ( x = r_rBounds[ 1 ].left; x <= r_rBounds[ 1 ].right; x ++ )
	{
		COLORREF *r_dSeek = &_PX( r, x, y );

		if ( *r_dSeek & CF_WHITE )
		{
			COLORREF *s_dSeek = &r_dSeek[ CF_SHADOW_DIST * CF_ENLARGE_FACTOR * ( 1 + r _X ) ];

			if ( ! *s_dSeek )
			{
				*s_dSeek = 0x7F000000;
			}

			for ( b = -DELTA; b <= DELTA; b ++ )
			for ( a = -DELTA; a <= DELTA; a ++ )
			{
				if ( a * a + b * b < DELTA * DELTA )
				{
					COLORREF *c_dSeek = &r_dSeek[ a + b * r _X ];

					if ( ! ( *c_dSeek & CF_WHITE ) )
					{
						POINT c_pHit;

						COLORREF *s_dSeek = &c_dSeek[ CF_SHADOW_DIST * CF_ENLARGE_FACTOR * ( 1 + r _X ) ];
						*c_dSeek = 0xFF000000;

						c_pHit.x = x + a;
						c_pHit.y = y + b;
						TestRect( &r_rBounds[ 2 ], &c_pHit );

						if ( ! *s_dSeek )
						{
							*s_dSeek = 0x3F000000;

							c_pHit.x += CF_SHADOW_DIST * CF_ENLARGE_FACTOR;
							c_pHit.y += CF_SHADOW_DIST * CF_ENLARGE_FACTOR;
							TestRect( &r_rBounds[ 2 ], &c_pHit );
						}
					}
				}
			}
		}
	}

	q_sz.cx = ( r_rBounds[ 2 ].right - r_rBounds[ 2 ].left + 1 ) / CF_ENLARGE_FACTOR;
	q_sz.cy = ( r_rBounds[ 2 ].bottom - r_rBounds[ 2 ].top + 1 ) / CF_ENLARGE_FACTOR;

	q = MakeCF_WS( MakeBM( q_sz.cx, q_sz.cy ) );

	for ( y = 0; y < q_sz.cy; y ++ )
	for ( x = 0; x < q_sz.cx; x ++ )
	{
		DWORD c, argb[ 4 ] = { 0 };

		for ( b = 0; b < CF_ENLARGE_FACTOR; b ++ )
		for ( a = 0; a < CF_ENLARGE_FACTOR; a ++ )
		{
			COLORREF *c_dSeek = &_PX( r, x * CF_ENLARGE_FACTOR + a + r_rBounds[ 2 ].left,
										 y * CF_ENLARGE_FACTOR + b + r_rBounds[ 2 ].top );

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
			for ( c = 0; c < 4; c ++ )
			{
				( (BYTE *) &_PX( q, x, y ) )[ c ] = (BYTE) ( argb[ c ] / ( CF_ENLARGE_FACTOR * CF_ENLARGE_FACTOR ) );
			}
		}
	}

	KillCF_WS( r );

	q->pRot.x = (float) ( r_pPlg[ 0 ].x - r_rBounds[ 2 ].left + (int) ( n->pRot.x * r_fXC ) + (int) ( n->pRot.y * r_fYC ) ) / CF_ENLARGE_FACTOR;
	q->pRot.y = (float) ( r_pPlg[ 0 ].y - r_rBounds[ 2 ].top  + (int) ( n->pRot.x * r_fXS ) + (int) ( n->pRot.y * r_fYS ) ) / CF_ENLARGE_FACTOR;

	return q;
}

int WINAPI _DllMainCRTStartup( HINSTANCE t_hInst, DWORD c_dReason, void *c_vReserved )
{
	if ( c_dReason == DLL_PROCESS_ATTACH )
	{
		if ( !c_hInst ) c_hInst = t_hInst;
	}

	return 1;
}
/*
float o_fAngle;

void AssembleRotatedCursor( HWND w_hWnd, POINT &c_pOld, POINT &c_pNew )
{
	const int fX = c_pNew.x - c_pOld.x;
	const int fY = c_pNew.y - c_pOld.y;

	c_pOld.x = c_pNew.x;
	c_pOld.y = c_pNew.y;

	const float t_fAngle = - atan2( (float) fX, (float) fY ) - (float) M_PI / 2;

	while( abs( o_fAngle - 2 * M_PI - t_fAngle ) < abs( o_fAngle - t_fAngle ) ) o_fAngle -= 2 * (float) M_PI;
	while( abs( o_fAngle + 2 * M_PI - t_fAngle ) < abs( o_fAngle - t_fAngle ) ) o_fAngle += 2 * (float) M_PI;

	if ( abs( o_fAngle - t_fAngle ) < CA_ROTATION_STEP ) o_fAngle = t_fAngle; else
	if ( t_fAngle < o_fAngle ) o_fAngle -= CA_ROTATION_STEP; else o_fAngle += CA_ROTATION_STEP;

	RECT oeu;
	HBITMAP gogos = gogo( o_fAngle, oeu );

	BLENDFUNCTION blenda = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };

	POINT t_pSrc = { oeu.left, oeu.top };
	POINT t_pDst = { c_pNew.x, c_pNew.y };
	SIZE  t_pSiz = { oeu.right - oeu.left, oeu.bottom - oeu.top };

	HDC w_hDC = MakeDC( gogos );

	UpdateLayeredWindow( w_hWnd, 0, &t_pDst, &t_pSiz, w_hDC, &t_pSrc, 0, &blenda, ULW_ALPHA );
	SetWindowPos( w_hWnd, HWND_TOPMOST, t_pDst.x, t_pDst.y, t_pSiz.cx, t_pSiz.cy, SWP_SHOWWINDOW );

	KillDC( w_hDC );

	DeleteObject( gogos );
}
}

*/
