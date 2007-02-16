#include <windows.h>
#define _USE_MATH_DEFINES
#include <math.h>

#define IF_BACKGROUND 0x000000
#define IF_CENTRPOINT 0xffffff

#define IF_ENLARGE_FACTOR 20
#define IF_ENLARGE_CHAOS 2.1f

#define IF_BLUR_RADIUS 2
#define IF_BLUR_RADIUS_SQ ( ( IF_BLUR_RADIUS * 2 + 1 ) * ( IF_BLUR_RADIUS * 2 + 1 ) )
#define IF_BLUR_ITERATIONS 4

#define IF_ROTATION_ANGLE_STEP ( M_PI / 16 )
#define IF_ROTATION_SCALE_BASE ( 4.0f / IF_ENLARGE_FACTOR )
#define IF_ROTATION_SCALE_STEP ( IF_ROTATION_SCALE_BASE / 64 )

typedef struct
{
	HBITMAP hIcon;
	POINT pSize;
	POINT pCenter;
} IF_HBM;

HDC MakeDC( HBITMAP x_hBm, HGDIOBJ &x_hOld )
{
	HDC x_hDC = CreateCompatibleDC( 0 );
	x_hOld = SelectObject( x_hDC, x_hBm );
	return x_hDC;
}

HGDIOBJ KillDC( HDC x_hDC, HGDIOBJ x_hOld )
{
	HGDIOBJ x_hRet = SelectObject( x_hDC, x_hOld );
	DeleteDC( x_hDC );
	return x_hRet;
}

HBITMAP Create32BppBitmap( int t_iX, int t_iY, COLORREF **t_pdPx )
{
	BITMAPINFO t_xBMI = { 0 };
	{
		t_xBMI.bmiHeader.biSize = sizeof( BITMAPINFOHEADER );
		t_xBMI.bmiHeader.biWidth = t_iX;
		t_xBMI.bmiHeader.biHeight = - t_iY;
		t_xBMI.bmiHeader.biPlanes = 1;
		t_xBMI.bmiHeader.biBitCount = 32;
		t_xBMI.bmiHeader.biCompression = BI_RGB;
		t_xBMI.bmiHeader.biSizeImage = t_iX * t_iY << 2;
	}

	return CreateDIBSection( 0, &t_xBMI, DIB_RGB_COLORS, (void **) t_pdPx, 0, 0 );
}

void IconFactory( HBITMAP o_hBm, IF_HBM *n_xIF )
{
	BITMAP o_xBm;
	GetObject( o_hBm, sizeof( BITMAP ), &o_xBm );

	int	sX = o_xBm.bmWidth * IF_ENLARGE_FACTOR;
	int	sY = o_xBm.bmHeight * IF_ENLARGE_FACTOR;

	COLORREF *r_dPx;
	HBITMAP r_hBm = Create32BppBitmap( sX, sY << 1, &r_dPx );

	HGDIOBJ o_hOld;
	HDC o_hDC = MakeDC( o_hBm, o_hOld );

	RGBQUAD gradients[ 2 ] = { 0 };

	for ( int y = 0; y < o_xBm.bmHeight; y ++ )
	for ( int x = 0; x < o_xBm.bmWidth; x ++ )
	{
		COLORREF o_dPx = GetPixel( o_hDC, x, y );

		if ( o_dPx == IF_BACKGROUND ) continue;
		if ( o_dPx == IF_CENTRPOINT )
		{
			n_xIF->pCenter.x = (long) ( ( (float) x + 0.5f ) * IF_ENLARGE_FACTOR );
			n_xIF->pCenter.y = (long) ( ( (float) y + 0.5f ) * IF_ENLARGE_FACTOR );
			continue;
		}

		if ( * (COLORREF *) &gradients[ 0 ] == 0 )		* (COLORREF *) &gradients[ 0 ] = o_dPx; else
		if ( * (COLORREF *) &gradients[ 0 ] != o_dPx )	* (COLORREF *) &gradients[ 1 ] = o_dPx;
	}

	srand( GetTickCount() );

	for ( int y = 0; y < o_xBm.bmHeight; y ++ )
	for ( int x = 0; x < o_xBm.bmWidth; x ++ )
	{
		COLORREF o_dPx = GetPixel( o_hDC, x, y );

		if ( ( o_dPx == * (COLORREF *) &gradients[ 0 ] ) || ( o_dPx == * (COLORREF *) &gradients[ 1 ] ) )
		{
			for ( int b = 0; b < IF_ENLARGE_FACTOR; b ++ )
			for ( int a = 0; a < IF_ENLARGE_FACTOR; a ++ )
			{
				float perc = (float) ( x * IF_ENLARGE_FACTOR + a ) / ( sX - 1 ) + IF_ENLARGE_CHAOS * ( (float) rand() / RAND_MAX - 0.5f );

				if ( perc < 0.0f ) perc = 0.0; else if ( perc > 1.0f ) perc = 1.0f;

				r_dPx[ x * IF_ENLARGE_FACTOR + a + ( y * IF_ENLARGE_FACTOR + b + sY ) * sX ] = RGB
				(
					(BYTE) ( gradients[ 1 ].rgbRed	 * perc + gradients[ 0 ].rgbRed   * ( 1.0f - perc ) ),
					(BYTE) ( gradients[ 1 ].rgbGreen * perc + gradients[ 0 ].rgbGreen * ( 1.0f - perc ) ),
					(BYTE) ( gradients[ 1 ].rgbBlue  * perc + gradients[ 0 ].rgbBlue  * ( 1.0f - perc ) )
				);
			}
		}
	}

	KillDC( o_hDC, o_hOld );

	int x0 = sX, x1 = 0, y0 = sY, y1 = 0;

	for ( int i = 0; i < IF_BLUR_ITERATIONS; i ++ )
	{
		if ( i ) memcpy( &r_dPx[ sX * sY ], r_dPx, sX * sY << 2 );

		for ( int y = IF_BLUR_RADIUS; y < sY - IF_BLUR_RADIUS - 1; y ++ )
		for ( int x = IF_BLUR_RADIUS; x < sX - IF_BLUR_RADIUS - 1; x ++ )
		{
			COLORREF *r_dSeek = &r_dPx[ x + ( y + sY ) * sX ];

			DWORD rgb[ 3 ] = { 0 };

			for ( int b = -IF_BLUR_RADIUS; b <= IF_BLUR_RADIUS; b ++ )
			for ( int a = -IF_BLUR_RADIUS; a <= IF_BLUR_RADIUS; a ++ )
			{
				COLORREF col = r_dSeek[ a + b * sX ];

				rgb[ 0 ] += GetRValue( col );
				rgb[ 1 ] += GetGValue( col );
				rgb[ 2 ] += GetBValue( col );
			}

			COLORREF outCol = RGB( rgb[ 0 ] / IF_BLUR_RADIUS_SQ, rgb[ 1 ] / IF_BLUR_RADIUS_SQ, rgb[ 2 ] / IF_BLUR_RADIUS_SQ );

			if ( outCol )
			{
				r_dSeek[ - sX * sY ] = outCol;

				if ( i == IF_BLUR_ITERATIONS - 1 )
				{
					if ( x0 > x ) x0 = x;
					if ( y0 > y ) y0 = y;
					if ( x1 < x ) x1 = x;
					if ( y1 < y ) y1 = y;
				}
			}
		}
	}

	int wX = n_xIF->pSize.x = x1 - x0 + 1;
	int wY = n_xIF->pSize.y = y1 - y0 + 1;

	n_xIF->pCenter.x -= x0;
	n_xIF->pCenter.y -= y0;

	COLORREF *n_dPx;
	n_xIF->hIcon = Create32BppBitmap( wX, wY, &n_dPx );

	for ( int y = y0; y <= y1; y ++ )
	for ( int x = x0; x <= x1; x ++ )
	{
		COLORREF col = r_dPx[ x + y * sX ];

		n_dPx[ x - x0 + ( y - y0 ) * wX ] =	col == IF_BACKGROUND ? 0xffffff : col;
	}

	DeleteObject( r_hBm );
}

float o_fAngle, o_fAccela;

void IFDrawRotatedInstance( HDC o_hDC, IF_HBM &t_xIF, POINT &t_pOld, POINT &t_pNew )
{
	const int fX = t_pNew.x - t_pOld.x;
	const int fY = t_pNew.y - t_pOld.y;

	const float t_fAngle = - atan2( (float) fX, (float) fY ) - (float) M_PI / 2;

	while( abs( o_fAngle - 2 * M_PI - t_fAngle ) < abs( o_fAngle - t_fAngle ) ) o_fAngle -= 2 * (float) M_PI;
	while( abs( o_fAngle + 2 * M_PI - t_fAngle ) < abs( o_fAngle - t_fAngle ) ) o_fAngle += 2 * (float) M_PI;

	if ( abs( o_fAngle - t_fAngle ) < IF_ROTATION_ANGLE_STEP ) o_fAngle = t_fAngle; else
	if ( t_fAngle < o_fAngle ) o_fAngle -= (float) IF_ROTATION_ANGLE_STEP; else o_fAngle += (float) IF_ROTATION_ANGLE_STEP;

	float t_fAccela = IF_ROTATION_SCALE_BASE - IF_ROTATION_SCALE_STEP * sqrt( (float) fX * fX + fY * fY );
	if ( t_fAccela < IF_ROTATION_SCALE_BASE / 2 ) t_fAccela = IF_ROTATION_SCALE_BASE / 2;

	if ( abs( o_fAccela - t_fAccela ) < IF_ROTATION_SCALE_STEP ) o_fAccela = t_fAccela; else
	if ( t_fAccela < o_fAccela ) o_fAccela -= (float) IF_ROTATION_SCALE_STEP; else o_fAccela += (float) IF_ROTATION_SCALE_STEP;

	HGDIOBJ t_hOld;
	HDC t_hDC = MakeDC( t_xIF.hIcon, t_hOld );
	{
		POINT pnts[ 3 ];

		float tX = t_xIF.pSize.x * o_fAccela;
		float tY = t_xIF.pSize.y * o_fAccela;

		double fXC = tX * cos( o_fAngle );
		double fXS = tX * sin( o_fAngle );
		double fYC = tY * cos( o_fAngle + M_PI / 2 );
		double fYS = tY * sin( o_fAngle + M_PI / 2 );

		float lX = (float) t_xIF.pCenter.x / t_xIF.pSize.x;
		float lY = (float)  t_xIF.pCenter.y / t_xIF.pSize.y;

		pnts[ 0 ].x = t_pNew.x - (int) ( lX * fXC + lY * fYC );
		pnts[ 0 ].y = t_pNew.y - (int) ( lX * fXS + lY * fYS );

		pnts[ 1 ].x = pnts[ 0 ].x + (int) fXC;
		pnts[ 1 ].y = pnts[ 0 ].y + (int) fXS;

		pnts[ 2 ].x = pnts[ 0 ].x + (int) fYC;
		pnts[ 2 ].y = pnts[ 0 ].y + (int) fYS;

		PlgBlt( o_hDC, pnts, t_hDC, 0, 0, t_xIF.pSize.x, t_xIF.pSize.y, 0, 0, 0 );
	}

	KillDC( t_hDC, t_hOld );

	t_pOld.x = t_pNew.x;
	t_pOld.y = t_pNew.y;
}
