#include "CursorAssembly.h"

#define MAX_HDC_DEPTH 10

HDC		hdcStash[ MAX_HDC_DEPTH ] = { 0 };
HGDIOBJ hgoStash[ MAX_HDC_DEPTH ];

HDC MakeDC( HBITMAP x_hBm )
{
	for ( BYTE i = 0; i < MAX_HDC_DEPTH; i ++ )
	{
		if ( !hdcStash[ i ] )
		{
			HDC x_hDC =
			hdcStash[ i ] = CreateCompatibleDC( 0 );
			hgoStash[ i ] = SelectObject( x_hDC, x_hBm );

			return x_hDC;
		}
	}

	MessageBox( 0, "HDC LEAK!!! <10>", 0, MB_ICONHAND );
	return 0;
}

HBITMAP KillDC( HDC x_hDC )
{
	for ( BYTE i = 0; i < MAX_HDC_DEPTH; i ++ )
	{
		if ( hdcStash[ i ] == x_hDC )
		{
			HGDIOBJ x_hRet = SelectObject( x_hDC, hgoStash[ i ] );
			DeleteDC( x_hDC );
			hdcStash[ i ] = 0;

			return (HBITMAP) x_hRet;
		}
	}

	MessageBox( 0, "HDC CRASH!!! <10>", 0, MB_ICONHAND );
	return 0;
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

	HBITMAP t_hRet = CreateDIBSection( 0, &t_xBMI, DIB_RGB_COLORS, (void **) t_pdPx, 0, 0 );


	return t_hRet;
}
