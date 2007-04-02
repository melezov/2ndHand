#ifndef _CURSORSHOP_H_
#define _CURSORSHOP_H_

	#include "SharedCode.h"

	#define WS_CURSORSHOP ( WS_POPUP | WS_VISIBLE )
	#define WX_CURSORSHOP ( WS_EX_PALETTEWINDOW | WS_EX_NOACTIVATE | WS_EX_LAYERED | WS_EX_TRANSPARENT )

	#define CS_WHITE		0x00FFFFFF
	#define CS_ALPHA_MASK	0xFF000000

	#define CS_SHADOW_OPAQ	0xFF0000FF
	#define	CS_SHADOW_DARK	0x5F005F00
	#define	CS_SHADOW_LITE	0x3F3F0000

	#define CS_ASPECT_CORRECTION 1.0f

	typedef struct
	{
		BITMAPINFOHEADER	bih;		// bitmap info header duh
		HBITMAP				hBM;		// cursor HBITMAP
		DWORD		       *dPx;		// pointer to bitmap bits (32 bpp)

		HDC					hDC;		// hDC in which the bitmap is loaded
		HGDIOBJ				hGO;		// old GDI object from the hDC

		RECT				rBd;	    // minimum bounding box
		union							// frame rotation point
        {
			POINT			iC;	        // used for rendered frames
			POINTFLOAT		fC;	        // used by cursor factory
		} pnt;
    }
	CS_FRAME;

    #define _x bih.biWidth
    #define _y bih.biHeight

	#define _rl rBd.left
	#define _rt rBd.top
	#define _rr rBd.right
	#define _rb rBd.bottom

    #define _fx pnt.fC.x
    #define _fy pnt.fC.y
	#define _ix pnt.iC.x
	#define _iy pnt.iC.y

    #define _PX( f, x, y ) ( ( ( f ) -> dPx ) [ ( x ) + ( y ) * ( ( f ) -> _x ) ] )

	typedef struct
	{
		TCHAR	resId[ MAX_PATH ];		// resource Id
		DWORD	rotSteps;				// number of frames in the factory
		SIZE	destSize;				// preferred cursor size
		BYTE	aaFactor;				// anti-aliasing factor x by x
		float	borderWidth;			// border width in destignation pixels
		float	shadowDist;				// shadow x and y distance in destignation pixels
		float	plasmaChaos;			// plasma dispersion factor
		DWORD	randSeed;				// random seed for color dispersion & co.
	}
	CS_HEADER;

    typedef struct
	{
		CS_HEADER			head;		// cursor factory information
		CS_FRAME    	   *rndy;		// rendered bordered plasma fractal
	}
	CS_FACTORY;

    CS_FRAME *MakeCS_Frame( int f_iX, int f_iY );
    CS_FRAME *LoadCS_Frame( CTRING path );
    CS_FRAME *PlsmCS_Frame( CS_FRAME *o, CS_HEADER *h );
    CS_FRAME *RendCS_Frame( CS_FACTORY *o, float fi );

    BOOL SaveCS_Frame( CS_FRAME *f, CTRING path );
    void TestCS_Frame( CS_FRAME *f, int t_iX, int t_iY );
    void KillCS_Frame( CS_FRAME *f );

    CS_FACTORY *MakeCS_Factory( CS_HEADER *h );
    void KillCS_Factory( CS_FACTORY *f );

#endif // _CURSORSHOP_H_
