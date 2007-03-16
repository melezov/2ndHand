#include "MainGUI.h"
#include "CursorShop.h"

CTRING defaultCursor = "..\\Cursor Bitmaps\\0x20 - Picker.bmp";

#include <stdio.h>

CS_FACTORY *f;
HWND cWnd;
POINT oPnt;

CTRING gc_sCN = TEXT( "2NDHAND_CURSOR_CLASS" );
CTRING gc_sWN = TEXT( "2ndHand - Cursor Window" );

LRESULT CALLBACK c_pProc( HWND c_hWnd, UINT c_uMsg, WPARAM c_wParam, LPARAM c_lParam )
{
	return DefWindowProc( c_hWnd, c_uMsg, c_wParam, c_lParam );
}

BOOL TestInit()
{
	BOOL iMake;
	CS_HEADER h;

	h.aaFactor = 3;
	h.randSeed = GetTickCount();
	h.borderWidth = 1.25f;
	h.destSize.cx = 40;
	h.destSize.cy = 32;
	h.plasmaChaos = 1.25f;
	h.rotSteps = 1300;
	h.shadowDist = 3.0f;
	wsprintf( h.resId, defaultCursor );

	iMake = MakeCS_Factory( &f, &h );
	if ( iMake ) return ERROR_CURSORSHOP( 0x50 );

	cWnd = CreateClassWindow( gc_sCN, gc_sWN, c_pProc, WS_CURSORSHOP, WX_CURSORSHOP );
	if ( !cWnd ) return ERROR_CURSORSHOP( 0x51 );

	return ERROR_SUCCESS;
}

float o_fAngle = 0, o_fRadius = 1.0f;

void TestCenter( UINT msg, WPARAM wparam, LPARAM lparam )
{
	if ( msg == WM_NCCREATE ) TestInit();

	if ( msg == WM_RATHOOK )
	{
		short nx = (short) LOWORD( lparam );
		short ny = (short) HIWORD( lparam );

		if ( cWnd )
		{
			BOOL iMake;
			CS_FRAME r;

			int fx = nx - oPnt.x;
			int fy = ny - oPnt.y;

			float t_fAngle = - (float) M_PI_2 - atan2f( fx, fy );
			float rr = 1.0f + sqrtf( fx * fx + fy * fy ) / 25.0f;

			float extra = rr > o_fRadius ? 1 : CS_STRETCH_SLUG;

			o_fRadius = ( o_fRadius * CS_STRETCH_SLUG + rr * extra ) / ( CS_STRETCH_SLUG + extra );

			while( abz( o_fAngle - 2 * (float) M_PI - t_fAngle ) < abz( o_fAngle - t_fAngle ) ) o_fAngle -= 2 * (float) M_PI;
			while( abz( o_fAngle + 2 * (float) M_PI - t_fAngle ) < abz( o_fAngle - t_fAngle ) ) o_fAngle += 2 * (float) M_PI;

			o_fAngle = ( o_fAngle * CS_ROTATION_SLUG + t_fAngle * o_fRadius ) / ( CS_ROTATION_SLUG + o_fRadius );

			iMake = RenderCS_Frame( &r, f, o_fAngle, o_fRadius );
			if ( iMake ) return ERROR_CURSORSHOP( 0x60 );
			{
				BLENDFUNCTION blenda = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
				POINT nPnt = { nx, ny };

				POINT t_pSrc = { 0, 0 };
				POINT t_pDst = { nPnt.x - _ix( &r ), nPnt.y - _iy( &r ) };
				SIZE  t_pSiz = { _x( &r ), _y( &r ) };

				UpdateLayeredWindow( cWnd, 0, &t_pDst, &t_pSiz, _d( &r ), &t_pSrc, 0, &blenda, ULW_ALPHA );
				SetWindowPos( cWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE );

				oPnt.x = nPnt.x;
				oPnt.y = nPnt.y;
			}

			iMake = KillCS_Frame( &r );
			if ( iMake ) return ERROR_CURSORSHOP( 0x61 );
		}
	}
}
