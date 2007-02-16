#ifndef _CURSOR_ASSEMBLY_H_
#define _CURSOR_ASSEMBLY_H_

#define _WIN32_WINNT 0x0500
#include <windows.h>

#define _USE_MATH_DEFINES
#include <math.h>

// ---------------------------------------------------------------------------

HDC MakeDC( HBITMAP x_hBm );
HBITMAP KillDC( HDC x_hDC );
HBITMAP Create32BppBitmap( int t_iX, int t_iY, COLORREF **t_pdPx );

// ---------------------------------------------------------------------------

typedef struct
{
	HDC hdc;
	HBITMAP hbm;
	COLORREF *bits;
	SIZE res;
	POINTFLOAT center;
	BYTE ID;
	POINT old;
	RGBQUAD grads[ 2 ];
} CA_INFO;

CA_INFO *CreateCursorFactory( BYTE resID );
void AssembleRotatedCursor( HWND w_hWnd, CA_INFO *curCA, POINT &c_pNew );


#endif // _CURSOR_ASSEMBLY_H_
