#include "WinGUI.h"
#include <Math.h>

typedef struct
{
	HBITMAP hIcon;
	POINT pSize;
	POINT pCenter;
} IF_HBM;

IF_HBM picker;

HDC MakeDC( HBITMAP x_hBm, HGDIOBJ &x_hOld );
HGDIOBJ KillDC( HDC x_hDC, HGDIOBJ x_hOld );
void IconFactory( HBITMAP o_hBm, IF_HBM *n_xIF );
HBITMAP Create32BppBitmap( int t_iX, int t_iY, COLORREF **t_pdPx );
void IFDrawRotatedInstance( HDC o_hDC, IF_HBM &t_xIF, POINT &t_pOld, POINT &t_pNew );

POINT w_pOld;

void BlitWindow( HWND w_hWnd, int t_iNx, int t_iNy )
{
	COLORREF *n_dBits;
	HBITMAP hhh = Create32BppBitmap( 500, 500, &n_dBits );

	HGDIOBJ ooo;
	HDC dcdc = MakeDC( hhh, ooo );

	POINT w_pNew = { 250 + t_iNx - w_pOld.x, 250 + t_iNy - w_pOld.y };

	IFDrawRotatedInstance( dcdc, picker, w_pOld, w_pNew );

	BLENDFUNCTION blenda = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };

	POINT t_pSrc = {  0,  0 };
	POINT t_pDst = { t_iNx, t_iNy };
	SIZE  t_pSiz = { 500, 500 };

	for ( int y = 0; y < 500; y ++ )
	for ( int x = 0; x < 500; x ++ )
	{

	}

	UpdateLayeredWindow( w_hWnd, 0, &t_pDst, &t_pSiz, dcdc, &t_pSrc, 0, &blenda, ULW_ALPHA );
	SetWindowPos( w_hWnd, HWND_TOPMOST, t_pDst.x, t_pDst.y, t_pSiz.cx, t_pSiz.cy, SWP_SHOWWINDOW );

	KillDC( dcdc, ooo );
	DeleteObject( hhh );
}

void IFStartup( HINSTANCE w_hInst )
{
	HBITMAP o_hBm = LoadBitmap( w_hInst, MAKEINTRESOURCE( 0x40 ) );
	IconFactory( o_hBm, &picker );
	DeleteObject( o_hBm );
}
