#ifndef _CURSORFACTORY_H_
#define _CURSORFACTORY_H_

#include "SharedIncludes.h"

#define _USE_MATH_DEFINES
#include <math.h>

#define _X .bmp.bmWidth
#define _Y .bmp.bmHeight

typedef struct
{
	HDC hDC;
	COLORREF *dPx;
	HBITMAP hBM;
	BITMAP bmp;
}
CF_WS; // CursorFactory_WorkSpace

HDC MakeDC( HBITMAP s_hBm );
HBITMAP KillDC( HDC s_hDC );

HBITMAP MakeBM( CF_WS &s );
void KillBM( CF_WS &s );

void TestRect( RECT &s_rBounds, POINT &s_pHit );
void AssembleRotatedCursor( HWND w_hWnd, POINT &c_pOld, POINT &c_pNew );
/*
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
*/

#endif // _CURSORFACTORY_H_
