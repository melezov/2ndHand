#ifndef _CURSORSHOP_H_
#define _CURSORSHOP_H_

	#include "SharedCode.h"

	#define _USE_MATH_DEFINES
	#include <math.h>

	#define WS_2NDHANDCURSOR ( WS_POPUP | WS_VISIBLE )
	#define WX_2NDHANDCURSOR ( WS_EX_PALETTEWINDOW | WS_EX_NOACTIVATE | WS_EX_LAYERED | WS_EX_TRANSPARENT )

	#define CS_BLACK 0x000000
	#define CS_WHITE 0xFFFFFF

	#define CS_SHADOW_OPAQ 0xFF000000
	#define	CS_SHADOW_DARK 0x5F000000
	#define	CS_SHADOW_LITE 0x3F000000

	#define CS_ASPECT_CORRECTION 1.0f

	typedef struct
	{
		TCHAR	resId[ MAX_PATH ];		// resource Id
		WORD	rotSteps;				// number of frames in the factory
		SIZE	destSize;				// preferred cursor size
		BYTE	aaFactor;				// anti-aliasing factor x by x
		float	borderWidth;			// border width in destignation pixels
		float	shadowDist;				// shadow x and y distance in destignation pixels
		DWORD	randSeed;				// random seed for color dispersion & co.
		float	gradChaos;				// color dispersion factor
	}
	CS_HEADER;

	typedef struct
	{
		HBITMAP				hBM;		// cursor HBITMAP
		COLORREF		   *dPx;		// pointer to bitmap bits (32 bpp)
		BITMAPINFOHEADER	bih;		// bitmap info header duh
	}
	CS_BITMAPINFO;

	#define _SIZEX bmi.bih.biWidth
	#define _SIZEY bmi.bih.biHeight * -1

	#define _DIBITS( c, x, y, o ) ( c ) o bmi.dPx[ ( x ) + ( y ) * ( c ) o _SIZEX ]

	#define _x . _SIZEX
	#define _y . _SIZEY

	#define _PD( c, x, y ) _DIBITS( c, x, y, . )

	#define _X -> _SIZEX
	#define _Y -> _SIZEY

	#define _PP( c, x, y ) _DIBITS( c, x, y, -> )

	typedef struct
	{
		POINT				pCenter;	// frame rotation point
		RECT				rBounds;	// minimum bounding box
	}
	CS_FRAMEPOSINFO;

	typedef struct
	{
		HDC					hDC;		// hDC in which the bitmap is loaded
		HGDIOBJ				hGO;		// old GDI object from the hDC
	}
	CS_DCINFO;

	typedef struct
	{
		CS_BITMAPINFO		bmi;		// bitmap info
		CS_DCINFO			dci;		// device context info
		CS_FRAMEPOSINFO		fpi;		// frame positioning info

		volatile BYTE		flag;		// current rendering state
	}
	CS_FRAME;

	#define CS_FLAG_IDLE	  0xFF
	#define CS_FLAG_INPROCESS 0x00
	#define CS_FLAG_RENDERED  0x01
	#define CS_FLAG_PRIORITY  0x02

	typedef struct
	{
		COLORREF	rgb;			// (a)rgb values
		DWORD		cnt;			// number of occurances
		POINTFLOAT	pnt;			// sum/average of x,y pos from colored pixels
	}
	CS_COLORINFO;

	#define CS_ORIGIN_MAX_COLORS 4
	#define CS_PLASMA_MAX_COLORS 256

	typedef struct
	{
		CS_HEADER		head;		// cursor factory information
		CS_FRAME		orig;		// original frame server

		CS_COLORINFO	cols[ CS_ORIGIN_MAX_COLORS ];
		CS_FRAME		plasma;		// plasma pattern

		HANDLE			mutex;
		CS_FRAME		rots[ 0 ];	// rendered frames
	}
	CS_FACTORY;

	void SaveCS_Frame( CS_FRAME *s, CTRING fileName );
	BOOL RaiseCS_Frame( CS_FRAME *f );
	BOOL MakeCS_Frame( CS_FRAME *f, int f_iX, int f_iY, BOOL f_oRaise );
	BOOL RenderCS_Frame( CS_FACTORY *f, WORD rotStep );
	void KillCS_Frame( CS_FRAME *f );

	void PlasmaCS_Frame( CS_FRAME *p, int grad, int *nr );
	BOOL InitRenderCS_Frame( CS_FACTORY *f, WORD rotStep, BOOL wait );
	void KillCS_Factory( CS_FACTORY **pf );
	CS_FACTORY *MakeCS_Factory( CS_HEADER *h );
	void RenderCS_Factory( CS_FACTORY *f );

#endif // _CURSORSHOP_H_
