#ifndef _CURSORFACTORY_H_
#define _CURSORFACTORY_H_

	#include "..\\SharedIncludes.h"

	#define _USE_MATH_DEFINES
	#include <math.h>

	#define CF_MAX_BIT_DEPTH 8

	typedef struct
	{
		COLORREF rgb;
		DWORD	 cnt;
		POINT	 pnt;
	}
	CF_CI; // CursorFactory_ColorInfo

	typedef struct
	{
		HBITMAP hBM;
		BITMAP bmp;

		HDC hDC;
		HGDIOBJ hGO;

		POINTFLOAT pRot;
	}
	CF_WS; // CursorFactory_WorkSpace

	#define _X ->bmp.bmWidth
	#define _Y ->bmp.bmHeight

	#define _PX( s, x, y ) ( (COLORREF *) s->bmp.bmBits )[ x + ( y ) * s _X ]

	#ifdef _MAINGUI_H_

		__declspec( dllimport ) CF_WS *LoadCF_WS( BYTE resID );                 // @1 NONAME
		__declspec( dllimport ) void KillCF_WS( CF_WS *k );		                // @2 NONAME
		__declspec( dllimport ) CF_WS *DeriveCF_WS( CF_WS *n, float r_fAngle ); // @3 NONAME

	#endif // _MAINGUI_H_

#endif // _CURSORFACTORY_H_
