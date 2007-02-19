#include "CursorFactory.h"

#define CF_BLACK 0x000000
#define CF_WHITE 0xFFFFFF

#define CF_CURSOR_XSIZE 48
#define CF_CURSOR_YSIZE 36

#define CF_ENLARGE_FACTOR 4
#define CF_BORDER_WIDTH 1.0f
#define CF_SHADOW_DIST 3

#define CF_MAX_BIT_DEPTH 8
#define CA_ROTATION_STEP ( (float) M_PI / 128 )

HBITMAP gogo( float r_fAngle, RECT &outty )
{
	CF_WS o, n;
	{
		o.hBM = (HBITMAP) LoadImage( 0, "..\\CursorFactory\\Pointer.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION );
		if ( !o.hBM ) return 0;

		GetObject( o.hBM, sizeof( BITMAP ), &o.bmp );
		o.hDC = MakeDC( o.hBM );

		n _X = CF_CURSOR_XSIZE * CF_ENLARGE_FACTOR;
		n _Y = CF_CURSOR_YSIZE * CF_ENLARGE_FACTOR;

		MakeBM( n );
	}

	DWORD o_dColInfo[ 4 << CF_MAX_BIT_DEPTH ] = { 0 };

	for ( int y = 0; y < o _Y; y ++ )
	for ( int x = 0; x < o _X; x ++ )
	{
		COLORREF t_dCol = GetPixel( o.hDC, x, y );

		if ( t_dCol == CF_BLACK ) continue;

		if ( t_dCol != CF_WHITE )
		{
			float fX = (float) n _X / o _X;
			float fY = (float) n _Y / o _Y;

			for ( int b = 0; b < fY; b ++ )
			for ( int a = 0; a < fX; a ++ )
			{
				n.dPx[ (int) ( x * fX + a ) + (int) ( y * fY + b ) * n _X ] = 0xffffffff;
			}
		}

		DWORD *t_dCurCI = o_dColInfo;

		while( 1 )
		{
			COLORREF t_dCur = *t_dCurCI;

			if ( !t_dCur || ( t_dCur == t_dCol ) )
			{
				t_dCurCI[ 0 ] = t_dCol;
				t_dCurCI[ 1 ] ++;
				t_dCurCI[ 2 ] += x;
				t_dCurCI[ 3 ] += y;

				break;
			}

			t_dCurCI += 4;
		}
	}

	for ( int i = 0; i < ( 1 << CF_MAX_BIT_DEPTH ) - 1; i ++ )
	for ( int r = i; r < ( 1 << CF_MAX_BIT_DEPTH );     r ++ )
	{
		DWORD *t_dCurCI = &o_dColInfo[ i << 2 ];
		DWORD *t_dCurCR = &o_dColInfo[ r << 2 ];

		if ( ( *t_dCurCR == CF_WHITE ) || ( t_dCurCI[ 1 ] < t_dCurCR[ 1 ] ) )
		{
			for ( int c = 0; c < 4; c ++ )
			{
				t_dCurCI[ c ] ^= t_dCurCR[ c ];
				t_dCurCR[ c ] ^= t_dCurCI[ c ];
				t_dCurCI[ c ] ^= t_dCurCR[ c ];
			}
		}
	}

	KillBM( o );

	for ( int y = 0; y < n _Y; y ++ )
	for ( int x = 0; x < n _X; x ++ )
	{
		COLORREF *n_dSeek = n.dPx + x + y * n _X;

		if ( ! *n_dSeek ) continue;

		float perc = (float) x / ( n _X - 1 );

		for ( BYTE rgb = 2; rgb != 0xff; rgb -- )
		{
			float grad = (float) ( (BYTE *) o_dColInfo ) [ 8 * 4 + 2 - rgb ] * perc +
						 (float) ( (BYTE *) o_dColInfo ) [ 4 * 4 + 2 - rgb ] * ( 1 - perc );

			if ( grad < 0.0f ) grad = 0.0f; else if ( grad > 255.0f ) grad = 255.0f;

			( (BYTE *) n_dSeek )[ rgb ] = (BYTE) grad;
		}
	}

	CF_WS r;
	{
		r _X = (int) ( CF_CURSOR_XSIZE + CF_CURSOR_YSIZE + CF_BORDER_WIDTH * 2 ) * CF_ENLARGE_FACTOR;
		r _Y = ( r _X * 3 ) >> 1;

		MakeBM( r );
	}

	float r_fXC = n _X * cos( r_fAngle );
	float r_fXS = n _X * sin( r_fAngle );
	float r_fYC = n _Y * cos( r_fAngle + (float) M_PI_2 );
	float r_fYS = n _Y * sin( r_fAngle + (float) M_PI_2 );

	POINT r_pPlg[ 4 ] = { 0 };
	{
		r_pPlg[ 1 ].x = (int) r_fXC;
		r_pPlg[ 1 ].y = (int) r_fXS;

		r_pPlg[ 2 ].x = (int) r_fYC;
		r_pPlg[ 2 ].y = (int) r_fYS;

		r_pPlg[ 3 ].x = (int) ( r_fXC + r_fYC );
		r_pPlg[ 3 ].y = (int) ( r_fXS + r_fYS );
	}

	#define DELTA ( CF_BORDER_WIDTH * CF_ENLARGE_FACTOR )

	RECT r_rBounds = { LONG_MAX, LONG_MAX, LONG_MIN, LONG_MIN };
	{
		for ( int i = 0; i < 4; i ++ )
		{
			TestRect( r_rBounds, r_pPlg[ i ] );
		}

		r_rBounds.left -= (long) DELTA;
		r_rBounds.top  -= (long) DELTA;

		for ( int i = 0; i < 3; i ++ )
		{
			r_pPlg[ i ].x -= r_rBounds.left;
			r_pPlg[ i ].y -= r_rBounds.top;
		}

		r_rBounds.right  -= r_rBounds.left;
		r_rBounds.bottom -= r_rBounds.top;
		r_rBounds.left    = 0;
		r_rBounds.top     = 0;
	}

	PlgBlt( r.hDC, r_pPlg, n.hDC, 0, 0, n _X, n _Y, 0, 0, 0 );

	RECT p_rBounds = { LONG_MAX, LONG_MAX, LONG_MIN, LONG_MIN };

	for ( int y = r_rBounds.top; y <= r_rBounds.bottom; y ++ )
	for ( int x = r_rBounds.left; x <= r_rBounds.right; x ++ )
	{
		COLORREF *r_dSeek = &r.dPx[ x + y * r _X ];

		if ( *r_dSeek & CF_WHITE )
		{
			COLORREF *s_dSeek = &r_dSeek[ CF_SHADOW_DIST * CF_ENLARGE_FACTOR * ( 1 + r _X ) ];

			if ( ! *s_dSeek )
			{
				*s_dSeek = 0x7F000000;
			}

			for ( int b = (int) -DELTA; b <= DELTA; b ++ )
			for ( int a = (int) -DELTA; a <= DELTA; a ++ )
			{
				if ( a*a + b*b < DELTA*DELTA )
				{
					COLORREF *c_dSeek = &r_dSeek[ a + b * r _X ];

					if ( ! ( *c_dSeek & CF_WHITE ) )
					{
						*c_dSeek = 0xFF000000;
						POINT c_pHit = { x + a, y + b };
						TestRect( p_rBounds, c_pHit );

						COLORREF *s_dSeek = &c_dSeek[ CF_SHADOW_DIST * CF_ENLARGE_FACTOR * ( 1 + r _X ) ];

						if ( ! *s_dSeek )
						{
							*s_dSeek = 0x3F000000;
							c_pHit.x += CF_SHADOW_DIST * CF_ENLARGE_FACTOR;
							c_pHit.y += CF_SHADOW_DIST * CF_ENLARGE_FACTOR;
							TestRect( p_rBounds, c_pHit );
						}
					}
				}
			}
		}
	}

	RECT q_rBounds = { LONG_MAX, LONG_MAX, LONG_MIN, LONG_MIN };

	for ( int y = p_rBounds.top; y <= p_rBounds.bottom; y += CF_ENLARGE_FACTOR )
	for ( int x = p_rBounds.left; x <= p_rBounds.right; x += CF_ENLARGE_FACTOR )
	{
		DWORD argb[ 4 ] = { 0 };

		for ( int b = 0; b < CF_ENLARGE_FACTOR; b ++ )
		for ( int a = 0; a < CF_ENLARGE_FACTOR; a ++ )
		{
			COLORREF *c_dSeek = &r.dPx[ x + a + ( y + b ) * r _X ];

			if ( *c_dSeek )
			{
				for ( int c = 0; c < 4; c ++ )
				{
					argb[ c ] += ( (BYTE *) c_dSeek )[ c ];
				}
			}
		}

		if ( !argb[ 3 ] ) continue;

		POINT q_pHit = { x / CF_ENLARGE_FACTOR, y / CF_ENLARGE_FACTOR + ( r _Y >> 1 ) };
		TestRect( q_rBounds, q_pHit );

		BYTE *r_bSeek = (BYTE *) &r.dPx[ q_pHit.x + q_pHit.y * r _X ];

		for ( int c = 0; c < 4; c ++ )
		{
			r_bSeek[ c ] = (BYTE) ( argb[ c ] / ( CF_ENLARGE_FACTOR * CF_ENLARGE_FACTOR ) );
		}
	}

	KillBM( n );
	KillDC( r.hDC );

	outty.left = 0;
	outty.top = 0;
	outty.right = r _X;
	outty.bottom = r _Y;

	return r.hBM;
}

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
