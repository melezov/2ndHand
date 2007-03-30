#ifndef _CURSORSHOP_H_
#define _CURSORSHOP_H_

	#include "SharedCode.h"

	#define WS_CURSORSHOP ( WS_POPUP | WS_VISIBLE )
	#define WX_CURSORSHOP ( WS_EX_PALETTEWINDOW | WS_EX_NOACTIVATE | WS_EX_LAYERED | WS_EX_TRANSPARENT )

	#define CS_WHITE		0x00FFFFFF
	#define CS_ALPHA_MASK	0xFF000000

	#define CS_SHADOW_OPAQ	0xFF000000
	#define	CS_SHADOW_DARK	0x5F000000
	#define	CS_SHADOW_LITE	0x3F000000

	#define CS_ASPECT_CORRECTION 1.0f

	typedef struct
	{
		BITMAPINFOHEADER	bih;		// bitmap info header duh
		HBITMAP				hBM;		// cursor HBITMAP
		COLORREF		   *dPx;		// pointer to bitmap bits (32 bpp)

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

	#define _PX( f, x, y ) ( f dPx )[ ( x ) + ( y ) * ( f _x ) ]

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

/*
	typedef struct
	{
		HANDLE				sema;		// synchronization semafor
		BYTE				status;		// current frame status
	}
	CS_SYNHRO;

	#define CS_SYNHRO_ABANDONED		0xFF
	#define CS_SYNHRO_IDLE			0x00
	#define CS_SYNHRO_PRIORITY		0x01
	#define CS_SYNHRO_PREVIEWING	0x02
	#define CS_SYNHRO_RENDERING		0x03
	#define CS_SYNHRO_FINISHED		0x04

	typedef struct
	{
		CS_SYNHRO			syn;		// thread insurance
		CS_FRAME			render;		// rendered frame
	}
	CS_SYNFRAME;

	typedef struct
	{
		COLORREF	rgb;			// (a)rgb values
		DWORD		cnt;			// number of occurances
	}
	CS_COLORINFO;

	typedef struct
	{
		CS_BITMAPCONTEXT	rsrc;
		CS_FRAME			orig;
	}
	CS_ORIGININFO;

	#define CS_ORIGIN_MAX_COLORS    4
	#define CS_PLASMA_MAX_COLORS 0xFF

	typedef struct
	{
		CS_BITMAPCONTEXT	plas;		// destignation plasma fractal
		CS_FRAME			flop;		// fractal with loaded color info

		DWORD				cNum;		// number of valid colors in cols array

		CS_COLORINFO		cols[ CS_ORIGIN_MAX_COLORS ];
		COLORREF			pals[ CS_PLASMA_MAX_COLORS ];
	}
	CS_PLASMAINFO;

	typedef struct
	{
		CS_FRAME			show;
	}
	CS_RENDERINFO;

	typedef struct
	{
		CS_HEADER			head;		// cursor factory information

		CS_ORIGININFO		orgy;		// resource & origin information
		CS_PLASMAINFO		plsy;		// rendering plasma fractal
		CS_RENDERINFO		rndy;		// the new factory source

		CS_SYNFRAME			rots[ 0 ];	// rendered frames
	}
	CS_FACTORY;

	BOOL MakeCS_BitmapInfo( CS_BITMAPINFO *f, int f_iX, int f_iY );
	BOOL KillCS_BitmapInfo( CS_BITMAPINFO *f );
	BOOL MakeCS_DCInfo( CS_DCINFO *f, HBITMAP h );
	BOOL KillCS_DCInfo( CS_DCINFO *f );
	BOOL MakeCS_BitmapContext( CS_BITMAPCONTEXT *f, int f_iX, int f_iY );
	BOOL KillCS_BitmapContext( CS_BITMAPCONTEXT *f );
	BOOL MakeCS_Frame( CS_FRAME *f, int f_iX, int f_iY );
	BOOL KillCS_Frame( CS_FRAME *f );
	BOOL MakeCS_Synhro( CS_SYNHRO *f );
	BOOL KillCS_Synhro( CS_SYNHRO *f );

	BOOL MakeCS_FramePosInfo( CS_FRAMEPOSINFO *f );
	BOOL KillCS_FramePosInfo( CS_FRAMEPOSINFO *f );

	void TestCS_FramePosInfo( CS_FRAMEPOSINFO *f, int t_iX, int t_iY );

	BOOL MakeCS_OriginInfo( CS_ORIGININFO *f, HBITMAP h );
	BOOL KillCS_OriginInfo( CS_ORIGININFO *f );

	BOOL MakeCS_PlasmaInfo( CS_PLASMAINFO *f, CS_HEADER *h, CS_FRAME *o );
	BOOL KillCS_PlasmaInfo( CS_PLASMAINFO *f );

	BOOL MakeCS_RenderInfo( CS_RENDERINFO *f, CS_HEADER *h, CS_FRAME *o );
	BOOL KillCS_RenderInfo( CS_RENDERINFO *f );

	BOOL MakeCS_Factory( CS_FACTORY **f, CS_HEADER *h );
	BOOL KillCS_Factory( CS_FACTORY **f );

	BOOL RenderCS_Frame( CS_FRAME *f, CS_FACTORY *o, float fi, float rr );
*/
#endif // _CURSORSHOP_H_
