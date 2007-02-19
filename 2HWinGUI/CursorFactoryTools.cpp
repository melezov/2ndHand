#include "CursorFactory.h"

#define MAX_HDC_DEPTH 10

HDC		s_hDCStash[ MAX_HDC_DEPTH ] = { 0 };
HGDIOBJ s_hGOStash[ MAX_HDC_DEPTH ];

HDC MakeDC( HBITMAP s_hBm )
{
	for ( BYTE i = 0; i < MAX_HDC_DEPTH; i ++ )
	{
		if ( !s_hDCStash[ i ] )
		{
			HDC t_hDC =
			s_hDCStash[ i ] = CreateCompatibleDC( 0 );
			s_hGOStash[ i ] = SelectObject( t_hDC, s_hBm );

			return t_hDC;
		}
	}

	MessageBox( 0, "HDC LEAK!!! <10>", 0, MB_ICONHAND | MB_OK );
	return 0;
}

HBITMAP KillDC( HDC s_hDC )
{
	for ( BYTE i = 0; i < MAX_HDC_DEPTH; i ++ )
	{
		if ( s_hDCStash[ i ] == s_hDC )
		{
			HGDIOBJ t_hRet = SelectObject( s_hDC, s_hGOStash[ i ] );
			DeleteDC( s_hDC );
			s_hDCStash[ i ] = 0;

			return (HBITMAP) t_hRet;
		}
	}

	MessageBox( 0, "HDC CRASH!!! <10>", 0, MB_ICONHAND | MB_OK );
	return 0;
}

HBITMAP MakeBM( CF_WS &s )
{
	BITMAPINFO t_xBMI = { 0 };
	{
		t_xBMI.bmiHeader.biSize = sizeof( BITMAPINFOHEADER );
		t_xBMI.bmiHeader.biWidth = s.bmp.bmWidth;
		t_xBMI.bmiHeader.biHeight = - s.bmp.bmHeight;
		t_xBMI.bmiHeader.biPlanes = 1;
		t_xBMI.bmiHeader.biBitCount = 32;
		t_xBMI.bmiHeader.biCompression = BI_RGB;
		t_xBMI.bmiHeader.biSizeImage = s.bmp.bmWidth * s.bmp.bmHeight << 2;
	}

	s.hBM = CreateDIBSection( 0, &t_xBMI, DIB_RGB_COLORS, (void **) &s.dPx, 0, 0 );
	s.hDC = MakeDC( s.hBM );

	return s.hBM;
}

void KillBM( CF_WS &s )
{
	KillDC( s.hDC );
	DeleteObject( s.hBM );
}

void TestRect( RECT &s_rBounds, POINT &s_pHit )
{
	if ( s_rBounds.left   > s_pHit.x ) s_rBounds.left   = s_pHit.x;
	if ( s_rBounds.top    > s_pHit.y ) s_rBounds.top    = s_pHit.y;
	if ( s_rBounds.right  < s_pHit.x ) s_rBounds.right  = s_pHit.x;
	if ( s_rBounds.bottom < s_pHit.y ) s_rBounds.bottom = s_pHit.y;
}
