#include "MainGUI.h"
#include "CursorShop.h"

CTRING defaultCursor = "..\\Cursor Bitmaps\\0x10 - Pointer.bmp";

void TestInit( HDC bDC )
{
	CS_FACTORY *f;
	CS_HEADER h;
	CS_BITMAPCONTEXT *t;

	BOOL iMake;
	BLENDFUNCTION blenda = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };

	h.aaFactor = 4;
	h.randSeed = GetTickCount();
	h.borderWidth = 1.25f;
	h.destSize.cx = 40;
	h.destSize.cy = 30;
	h.plasmaChaos = 1.0f;
	h.rotSteps = 32;
	h.shadowDist = 3.0f;
	wsprintf( h.resId, defaultCursor );

	iMake = MakeCS_Factory( &f, &h );
	iMake = 0;

	t = &f->orgy.rsrc;
	BitBlt( bDC, iMake, 0, _x( t ), _y( t ), _d( t ), 0, 0, SRCCOPY );
	iMake += _x( t ) + 1;

	t = &f->orgy.orig;
	BitBlt( bDC, iMake, 0, _x( t ), _y( t ), _d( t ), 0, 0, SRCCOPY );
	iMake += _x( t ) + 1;

	t = &f->orgy.crop;
	BitBlt( bDC, iMake, 0, _x( t ), _y( t ), _d( t ), 0, 0, SRCCOPY );
	iMake += _x( t ) + 1;

	t = &f->plsy.plas;
	BitBlt( bDC, iMake, 0, _x( t ), _y( t ), _d( t ), 0, 0, SRCCOPY );
	iMake += _x( t ) + 1;

	t = &f->plsy.flop;
	AlphaBlend( bDC, iMake, 0, _x( t ), _y( t ), _d( t ), 0, 0, _x( t ), _y( t ), blenda );
	iMake += _x( t ) + 1;

	t = &f->rndy.show;
//	BitBlt( bDC, iMake, 0, _x( t ), _y( t ), _d( t ), 0, 0, SRCCOPY );
	AlphaBlend( bDC, iMake, 0, _x( t ), _y( t ), _d( t ), 0, 0, _x( t ), _y( t ), blenda );
	iMake += _x( t ) + 1;

	t = &f->rots[ 1 ].preview.bcx;
//	BitBlt( bDC, iMake, 0, _x( t ), _y( t ), _d( t ), 0, 0, SRCCOPY );
	AlphaBlend( bDC, iMake, 0, _x( t ), _y( t ), _d( t ), 0, 0, _x( t ), _y( t ), blenda );
	iMake += _x( t ) + 1;

	t = &f->rots[ 0 ].preview.bcx;
//	BitBlt( bDC, iMake, 0, _x( t ), _y( t ), _d( t ), 0, 0, SRCCOPY );
	AlphaBlend( bDC, iMake, 0, _x( t ), _y( t ), _d( t ), 0, 0, _x( t ), _y( t ), blenda );
	iMake += _x( t ) + 1;

	KillCS_Factory( &f );
}

void TestCenter( UINT msg, WPARAM wparam, LPARAM lparam )
{
	if ( msg == WM_NCCREATE ) TestInit( 0 );

	if ( msg == WM_PAINT )
	{
		static HDC bDC = 0;

		PAINTSTRUCT x_PS;
		HDC tDC = BeginPaint( gm_hWnd, &x_PS );
		RECT tRect;

		int rX = GetSystemMetrics( SM_CXSCREEN );
		int rY = GetSystemMetrics( SM_CYSCREEN );

		GetClientRect( gm_hWnd, &tRect );

		if ( bDC == 0 )
		{
			bDC = CreateCompatibleDC( 0 );
			SelectObject( bDC, CreateCompatibleBitmap( tDC, rX, rY ) );
			SelectObject( bDC, CreateSolidBrush( 0xAFAFFF ) ) ;
		}

		BitBlt( bDC, 0, 0, rX, rY, bDC, 0, 0, PATCOPY );
		TestInit( bDC );

		BitBlt( tDC, 0, 0, tRect.right, tRect.bottom, bDC, 0, 0, SRCCOPY );
		EndPaint( gm_hWnd, &x_PS );
	}

	else if ( msg == WM_RATHOOK )
	{
		WORD r_uMsg = LOWORD( wparam );
		static HDC dDC = 0;
		static int x, y, z;
		int nx, ny;
		static COLORREF col = 0xFF;
		BYTE resId = 0;

		InvalidateRect( gm_hWnd, 0, 0 );

		if ( !dDC ) dDC = GetDC( 0 );

		switch ( r_uMsg )
		{
			case	WM_LBUTTONDOWN:
			case	WM_MBUTTONDOWN:
			case	WM_RBUTTONDOWN: resId = IDI_PICKER; col = 0xFF0000; break;

			case	WM_LBUTTONUP:
			case	WM_MBUTTONUP:
			case	WM_RBUTTONUP: resId = IDI_POINTER; col = 0xFF;
		}

		if ( resId ) TrayAction( LoadIcon( gm_hInst, MAKEINTRESOURCE( resId ) ), 0, NIM_MODIFY );

		nx = (short) LOWORD( lparam );
		ny = (short) HIWORD( lparam );

		if ( !z )
		{
			x = nx;
			y = ny;
			z = 1;
		}

		if ( ( x != nx ) || ( y != ny ) )
		{
			int x0 = x;
			int x1 = nx;

			int y0 = y;
			int y1 = ny;

			BOOL steep = abs(y1 - y0) > abs(x1 - x0);

			if ( steep )
			{
				y0 ^= x0 ^= y0 ^= x0;
				x1 ^= y1 ^= x1 ^= y1;
			}

			if ( x0 > x1 )
			{
				x0 ^= x1 ^= x0 ^= x1;

				if ( y0 != y1 )
				y0 ^= y1 ^= y0 ^= y1;
			}

			{
			int deltax = x1 - x0;
			int deltay = abs(y1 - y0);

			int y = y0;
			int ystep = y0 < y1 ?  1 : -1;

			int x;

			float error = 0;
			float deltaerr = (float) deltay / deltax;

			 for ( x = x0; x <= x1; x ++ )
			{
				if ( steep )
				{
					SetPixelV( dDC, y, x, col >> 1);
				}
				else
				{
					SetPixelV( dDC, x, y, col >> 1 );
				}

				error += deltaerr;

				if  (error >= 0.5f )
				{
					y += ystep;
					error -= 1.0f;
				}
			 }
		 }
		}

//		MoveToEx( dDC, x, y, 0 );
//		LineTo( dDC, nx, ny );

		SetPixelV( dDC, x-1, y, col );
		SetPixelV( dDC, x+1, y, col );
		SetPixelV( dDC, x, y-1, col );
		SetPixelV( dDC, x, y+1, col );

		x = nx;
		y = ny;
	}
}
