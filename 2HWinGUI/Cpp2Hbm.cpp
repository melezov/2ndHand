#include "WinGUI.h"
#include <Math.h>

extern unsigned char Pointers[];

typedef struct
{
	BYTE i_bX, i_bY;
	BYTE i_pX, i_pY;
	DWORD i_bARGB[ 1 ];
} ICON_DESC;

int t_iOx, t_iOy;
BYTE t_bOld;

char CloseNess( char t_cDelta, BYTE t_bDegrees )
{
	BYTE t_bNew = ( t_bOld + t_cDelta + 36 ) % 36;

	char t_cDiff1 = abs( t_bDegrees - t_bNew );
	char t_cDiff2 = t_bNew < t_bDegrees ? abs( t_bNew + 36 - t_bDegrees ) : abs( t_bDegrees + 36 - t_bNew );

	return t_cDiff1 < t_cDiff2 ? t_cDiff1 : t_cDiff2;
}

void BlitWindow0( HWND w_hWnd, int t_iNx, int t_iNy )
{
	const int fX = t_iNx - t_iOx;
	const int fY = t_iNy - t_iOy;

	const double t_fAngle = atan2( (double) fX, (double) fY );
	const BYTE t_bDegrees = (BYTE) ( ( (int) ( 1 - ( t_fAngle / 3.1416 ) * 180 + 270 ) % 360 ) / 10 );

	if ( t_bOld != t_bDegrees )
	{
		if ( CloseNess( 1, t_bDegrees ) < CloseNess( -1, t_bDegrees ) ) t_bOld ++; else t_bOld --;

		t_bOld += 36;
		t_bOld %= 36;
	}

	ICON_DESC *t_iCurr = (ICON_DESC *) ( Pointers + ( (DWORD *) Pointers )[ t_bOld ] );

    BITMAPINFO t_xBMI = { 0 };
    {
        t_xBMI.bmiHeader.biSize = sizeof( BITMAPINFOHEADER );
        t_xBMI.bmiHeader.biWidth = t_iCurr->i_bX;
        t_xBMI.bmiHeader.biHeight = - t_iCurr->i_bY;
        t_xBMI.bmiHeader.biPlanes = 1;
        t_xBMI.bmiHeader.biBitCount = 32;
        t_xBMI.bmiHeader.biCompression = BI_RGB;
        t_xBMI.bmiHeader.biSizeImage = ( t_iCurr->i_bX * t_iCurr->i_bY ) << 2;
    }

    BYTE *t_vBits;
    HBITMAP t_hBM = CreateDIBSection( 0, &t_xBMI, DIB_RGB_COLORS, (void **) &t_vBits, 0, 0 );

	RtlCopyMemory( t_vBits, &t_iCurr->i_bARGB, t_xBMI.bmiHeader.biSizeImage );

	HDC t_hDC = CreateCompatibleDC( 0 );
    SelectObject( t_hDC, t_hBM );

	BLENDFUNCTION blenda = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };

	POINT t_pSrc = {  0,  0 };
	POINT t_pDst = { t_iNx - t_iCurr->i_pX, t_iNy - t_iCurr->i_pY };
	SIZE  t_pSiz = { t_iCurr->i_bX, t_iCurr->i_bY };

	UpdateLayeredWindow( w_hWnd, 0, &t_pDst, &t_pSiz, t_hDC, &t_pSrc, 0, &blenda, ULW_ALPHA );
	SetWindowPos( w_hWnd, HWND_TOPMOST, t_pDst.x, t_pDst.y, t_pSiz.cx, t_pSiz.cy, SWP_SHOWWINDOW );

	DeleteDC( t_hDC );
	DeleteObject( t_hBM );

	t_iOx = t_iNx;
	t_iOy = t_iNy;
}
