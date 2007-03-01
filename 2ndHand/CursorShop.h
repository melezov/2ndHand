#ifndef _CURSORSHOP_H_
#define _CURSORSHOP_H_

	#include "..\\SharedIncludes.h"

	#define _USE_MATH_DEFINES
	#include <math.h>

	#define CS_BLACK 0x000000
	#define CS_WHITE 0xFFFFFF

	#define	CS_SHADOW_DARK	0x5F000000
	#define	CS_SHADOW_LIGHT 0x3F000000

	#define CS_ALPHA_MASK 0xff000000

	typedef struct
	{
		BYTE	cursorId;			// cursor resource ID
		WORD	rotSteps;			// number of frames in the factory
		SIZE	destSize;			// preferred cursor size
		BYTE	aaFactor;			// anti-aliasing factor x by x
		float	borderWidth;		// border width in destignation pixels
		float	shadowDist;			// shadow x and y distance in destignation pixels
		DWORD	randSeed;			// random seed for color dispersion & co.
		float	gradChaos;			// color dispersion factor
	}
	CS_HEADER;

	typedef struct
	{
		HBITMAP				hBM;	// cursor HBITMAP
		COLORREF		   *dPx;	// pointer to bitmap bits (32 bpp)
		BITMAPINFOHEADER	bih;	// bitmap info header duh
		HDC					hDC;	// hDC in which the bitmap is loaded
		HGDIOBJ				hGO;	// old GDI object from the hDC
	}
	CS_FRAME;

	#define _X ->bih.biWidth
	#define _Y ->bih.biHeight * -1

	#define _PX( c, x, y ) ( c )->dPx[ ( x ) + ( y ) * ( c ) _X ]

	typedef struct
	{
		COLORREF	rgb;			// (a)rgb values
		DWORD		cnt;			// number of occurances
		POINTFLOAT	pnt;			// sum/average of x,y pos from colored pixels
	}
	CS_COLORINFO;

	#define CS_MAX_COLORS 4

	typedef struct
	{
		POINT	hotSpot;			// frame rotation point
		RECT	bounds;				// minimum bounding box
	}
	CS_DIMINFO;

	typedef struct
	{
		CS_HEADER		header;						// cursor factory information
		CS_FRAME		origin;						// original frame server
		CS_COLORINFO	palette[ CS_MAX_COLORS ];	// origin's palettes
		CS_FRAME		stretch;					// source for rendering
		HANDLE			plgMutex;					// mutex for PlgBlt critical section
		CS_FRAME		grid;						// rendered frames grid
		SIZE			maxFrame;					// maximum frame dimensions
		CS_DIMINFO		dims[ 0 ];					// frame dimension info
	}
	CS_FACTORY;

	typedef struct
	{
		CS_HEADER	header;
		CS_FRAME	dump;
		CS_FRAME	thumb;
		SIZE		minFrame;
		POINT		hotSpots[ 0 ];
	}
	CS_DUMP;

	#define CS_PARAMETAR_HAX 3

	void TestRect( RECT *c_rBounds, POINT *c_pHit );
	void SaveCS_Frame( CS_FRAME *s, LPCSTR fileName );

#endif // _CURSORSHOP_H_
