#include "CursorAssembly.h"

#define CA_BACKGROUND 0x000000
#define CA_CENTRPOINT 0xffffff

#define CA_ENLARGE_FACTOR 4
#define CA_ROTATION_SCALE ( 3.0f / CA_ENLARGE_FACTOR )
#define CA_ROTATION_STEP ( (float) M_PI / 16 )

#define CA_ENLARGE_CHAOS 0.25f
#define CA_BLUR_RADIUS 1
#define CA_SHADOW_DISTANCE 4

extern HINSTANCE g_hInst;

CA_INFO *CreateCursorFactory( BYTE resID )
{
	CA_INFO *newCA;
	int nX, nY;
	{
		HBITMAP o_hBm = LoadBitmap( g_hInst, MAKEINTRESOURCE( resID ) );
		if ( !o_hBm ) return 0;

		int oX, oY;
		{
			BITMAP o_xBm;
			GetObject( o_hBm, sizeof( BITMAP ), &o_xBm );

			oX = o_xBm.bmWidth;
			oY = o_xBm.bmHeight;
		}

		newCA = (CA_INFO *) VirtualAlloc( 0, sizeof( CA_INFO ), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE );
		newCA->ID = resID;

		nX = newCA->res.cx = oX * CA_ENLARGE_FACTOR;
		nY = newCA->res.cy = oY * CA_ENLARGE_FACTOR;

		newCA->hbm = Create32BppBitmap( nX, nY, &newCA->bits );

		HDC o_hDC = MakeDC( o_hBm );
		{
			for ( int y = 0; y < oY; y ++ )
			for ( int x = 0; x < oX; x ++ )
			{
				COLORREF o_dPx = GetPixel( o_hDC, x, y );

				if ( o_dPx == CA_BACKGROUND ) continue;
				if ( o_dPx == CA_CENTRPOINT )
				{
					newCA->center.x = ( (float) x + 0.5f ) / oX;
					newCA->center.y = ( (float) y + 0.5f ) / oY;
					continue;
				}

				if ( * (COLORREF *) &newCA->grads[ 0 ] == 0 )		* (COLORREF *) &newCA->grads[ 0 ] = o_dPx; else
				if ( * (COLORREF *) &newCA->grads[ 0 ] != o_dPx )	* (COLORREF *) &newCA->grads[ 1 ] = o_dPx;

				for ( int b = 0; b < CA_ENLARGE_FACTOR; b ++ )
				{
					memset( &newCA->bits[ x * CA_ENLARGE_FACTOR + ( y * CA_ENLARGE_FACTOR + b ) * nX ], CA_CENTRPOINT, CA_ENLARGE_FACTOR << 2 );
				}
			}
		}

		KillDC( o_hDC );
		DeleteObject( o_hBm );
	}

	newCA->hdc = MakeDC( newCA->hbm );
	return newCA;
}

float o_fAngle;

void AssembleRotatedCursor( HWND w_hWnd, CA_INFO *curCA, POINT &c_pNew )
{
	const int fX = c_pNew.x - curCA->old.x;
	const int fY = c_pNew.y - curCA->old.y;

	curCA->old.x = c_pNew.x;
	curCA->old.y = c_pNew.y;

	const float t_fAngle = - atan2( (float) fX, (float) fY ) - (float) M_PI / 2;

	while( abs( o_fAngle - 2 * M_PI - t_fAngle ) < abs( o_fAngle - t_fAngle ) ) o_fAngle -= 2 * (float) M_PI;
	while( abs( o_fAngle + 2 * M_PI - t_fAngle ) < abs( o_fAngle - t_fAngle ) ) o_fAngle += 2 * (float) M_PI;

	if ( abs( o_fAngle - t_fAngle ) < CA_ROTATION_STEP ) o_fAngle = t_fAngle; else
	if ( t_fAngle < o_fAngle ) o_fAngle -= CA_ROTATION_STEP; else o_fAngle += CA_ROTATION_STEP;

	float tX = curCA->res.cx * CA_ROTATION_SCALE;
	float tY = curCA->res.cy * CA_ROTATION_SCALE;

	double fXC = tX * cos( o_fAngle );
	double fXS = tX * sin( o_fAngle );
	double fYC = tY * cos( o_fAngle + M_PI / 2 );
	double fYS = tY * sin( o_fAngle + M_PI / 2 );

	POINT pnts[ 4 ] = { 0 };
	{
		pnts[ 1 ].x = (int) fXC;
		pnts[ 1 ].y = (int) fXS;

		pnts[ 2 ].x = (int) fYC;
		pnts[ 2 ].y = (int) fYS;

		pnts[ 3 ].x = pnts[ 1 ].x + pnts[ 2 ].x;
		pnts[ 3 ].y = pnts[ 1 ].y + pnts[ 2 ].y;
	}

	RECT r_xRct = { curCA->res.cx, curCA->res.cy, 0, 0 };

	for ( int i = 0; i < 4; i ++ )
	{
		if ( r_xRct.left   > pnts[ i ].x ) r_xRct.left   = pnts[ i ].x;
		if ( r_xRct.top    > pnts[ i ].y ) r_xRct.top    = pnts[ i ].y;
		if ( r_xRct.right  < pnts[ i ].x ) r_xRct.right  = pnts[ i ].x;
		if ( r_xRct.bottom < pnts[ i ].y ) r_xRct.bottom = pnts[ i ].y;
	}

	for ( int i = 0; i < 4; i ++ )
	{
		pnts[ i ].x -= r_xRct.left;
		pnts[ i ].y -= r_xRct.top;
	}

	int wX = r_xRct.right - r_xRct.left;
	int wY = r_xRct.bottom - r_xRct.top;

	COLORREF *w_dPx;
	HBITMAP w_hBm = Create32BppBitmap( wX, wY << 1, &w_dPx );

	HDC w_hDC = MakeDC( w_hBm );
	{
		PlgBlt( w_hDC, pnts, curCA->hdc, 0, 0, curCA->res.cx, curCA->res.cy, 0, 0, 0 );
	}

	srand( GetTickCount() );

	for ( int y = 0; y < wY; y ++ )
	for ( int x = 0; x < wX; x ++ )
	{
		COLORREF *w_dSeek = &w_dPx[ x + y * wX ];

		if ( !*w_dSeek ) continue;

		float perc = (float) x / ( wX - 1 ) + CA_ENLARGE_CHAOS * ( (float) rand() / RAND_MAX - 0.5f );
		if ( perc < 0.0f ) perc = 0.0; else if ( perc > 1.0f ) perc = 1.0f;

		w_dPx[ x + ( y + wY ) * wX ] = RGB
		(
			(BYTE) ( curCA->grads[ 1 ].rgbRed	* perc + curCA->grads[ 0 ].rgbRed   * ( 1.0f - perc ) ),
			(BYTE) ( curCA->grads[ 1 ].rgbGreen * perc + curCA->grads[ 0 ].rgbGreen * ( 1.0f - perc ) ),
			(BYTE) ( curCA->grads[ 1 ].rgbBlue  * perc + curCA->grads[ 0 ].rgbBlue  * ( 1.0f - perc ) )
		);
	}

	for ( int y = 0; y < wY; y ++ )
	for ( int x = 0; x < wX; x ++ )
	{
		COLORREF col = w_dPx[ x + ( y + wY ) * wX ];

		BYTE val = ( GetRValue( col ) + GetGValue( col ) + GetBValue( col ) ) / 3;

		if ( val < 0x2f )
		{
			DWORD rgb[ 3 ] = { 0 };

			int cnt = 0;

			for ( int b = -CA_BLUR_RADIUS; b <= CA_BLUR_RADIUS; b ++ )
			for ( int a = -CA_BLUR_RADIUS; a <= CA_BLUR_RADIUS; a ++ )
			{
				if ( ( x + a < 0 ) || ( x + a >= wX ) ) continue;
				if ( ( y + b < 0 ) || ( y + b >= wY ) ) continue;

				cnt ++;

				col = w_dPx[ x + a + ( b + y + wY ) * wX ];

				rgb[ 0 ] += GetRValue( col );
				rgb[ 1 ] += GetGValue( col );
				rgb[ 2 ] += GetBValue( col );
			}

			col = RGB( rgb[ 0 ] / cnt, rgb[ 1 ] / cnt, rgb[ 2 ] / cnt );
		}

		w_dPx[ x + y * wX ] = col;
	}

	for ( int y = wY - 1; y >= 0; y -- )
	for ( int x = 0; x < wX; x ++ )
	{
		COLORREF *w_dSeek = &w_dPx[ x + y * wX ];

		if ( !*w_dSeek )
		{
			if ( ( x >= CA_SHADOW_DISTANCE ) && ( y >= CA_SHADOW_DISTANCE ) )
			{
				COLORREF col = w_dSeek[ - CA_SHADOW_DISTANCE * ( wX + 1 ) ];

				if ( col )
				{
					*w_dSeek = ( GetRValue( col ) + GetGValue( col ) + GetBValue( col ) ) / 3 * 0x01000000;
				}
				else *w_dSeek = 0;
			}
		} else *w_dSeek |= 0xff000000;
	}

	int qX = pnts[ 0 ].x + (int) ( curCA->center.x * fXC + curCA->center.y * fYC );
	int qY = pnts[ 0 ].y + (int) ( curCA->center.x * fXS + curCA->center.y * fYS );

	BLENDFUNCTION blenda = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };

	POINT t_pSrc = { 0, 0 };
	POINT t_pDst = { c_pNew.x - qX, c_pNew.y - qY };
	SIZE  t_pSiz = { wX, wY };

	UpdateLayeredWindow( w_hWnd, 0, &t_pDst, &t_pSiz, w_hDC, &t_pSrc, 0, &blenda, ULW_ALPHA );
	SetWindowPos( w_hWnd, HWND_TOPMOST, t_pDst.x, t_pDst.y, t_pSiz.cx, t_pSiz.cy, SWP_SHOWWINDOW );

	KillDC( w_hDC );
	DeleteObject( w_hBm );
}
