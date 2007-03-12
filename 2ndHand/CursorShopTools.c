#include "CursorShop.h"

void SaveCS_Frame( CS_FRAME *s, CTRING fileName )
{
	BITMAPFILEHEADER bmfh = { 0 };
	DWORD written;

	HANDLE file = CreateFile( fileName, GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_ARCHIVE, 0 );

	bmfh.bfType = MAKEWORD( 'B', 'M' );
    bmfh.bfOffBits = sizeof( BITMAPFILEHEADER ) + sizeof( BITMAPINFOHEADER );
	bmfh.bfSize = bmfh.bfOffBits + s->bmi.bih.biSizeImage;

	WriteFile( file, &bmfh, sizeof( BITMAPFILEHEADER ), &written, 0 );
	WriteFile( file, &s->bmi.bih, sizeof( BITMAPINFOHEADER ), &written, 0 );
	WriteFile( file, s->bmi.dPx, s->bmi.bih.biSizeImage, &written, 0 );

	CloseHandle( file );
}

BOOL RaiseCS_Frame( CS_FRAME *f )
{
	if ( !f->bmi.hBM ) return ERROR_CURSORSHOP( 1 );

	f->dci.hDC = CreateCompatibleDC( 0 );
	if ( !f->dci.hDC ) return ERROR_CURSORSHOP( 2 );

	f->dci.hGO = SelectObject( f->dci.hDC, f->bmi.hBM );
	if ( !f->dci.hGO ) return ERROR_CURSORSHOP( 3 );

	return ERROR_SUCCESS;
}

BOOL MakeCS_Frame( CS_FRAME *f, int f_iX, int f_iY, BOOL f_oRaise )
{
	memset( f, 0, sizeof( CS_FRAME ) );

	f->bmi.bih.biSize = sizeof( BITMAPINFOHEADER );
	f->bmi.bih.biPlanes = 1;
	f->bmi.bih.biBitCount = 32;
	f->bmi.bih.biWidth = f_iX;
	f->bmi.bih.biHeight = - f_iY;
	f->bmi.bih.biSizeImage = f_iX * f_iY << 2;

	f->bmi.hBM = CreateDIBSection( 0, (BITMAPINFO *) &f->bmi.bih, 0, &f->bmi.dPx, 0, 0 );

	if ( f_oRaise )
	{
		f_oRaise = RaiseCS_Frame( f );

		if ( f_oRaise )
		{
			KillCS_Frame( f );
			return f_oRaise;
		}
	}

	return ERROR_SUCCESS;
}

void KillCS_Frame( CS_FRAME *f )
{
	if ( f->dci.hDC )
	{
		if ( f->dci.hGO ) SelectObject( f->dci.hDC, f->dci.hGO );

		DeleteDC( f->dci.hDC );
	}

	if ( f->bmi.hBM )
	{
		DeleteObject( f->bmi.hBM );
	}

	memset( f, 0, sizeof( CS_BITMAPINFO ) + sizeof( CS_DCINFO ) );
}

void KillCS_Factory( CS_FACTORY **pf )
{
	CS_FACTORY *f = *pf;

	if ( f )
	{
		WORD rotSteps;

		KillCS_Frame( &f->orig );
		KillCS_Frame( &f->plasma );

		for ( rotSteps = 0; rotSteps < f->head.rotSteps; rotSteps ++ )
		{
			KillCS_Frame( &f->rots[ rotSteps ] );
		}

		if ( f->mutex ) CloseHandle( f->mutex );

		VirtualFree( f, 0, MEM_RELEASE );
		*pf = 0;
	}
}

BOOL EnumCS_ColorInfo( CS_FRAME *o, CS_COLORINFO *cI )
{
	int x, y;

	memset( cI, 0, sizeof( CS_COLORINFO ) * CS_ORIGIN_MAX_COLORS );

	for ( y = o _Y - 1; y >= 0; y -- )
	for ( x = o _X - 1; x >= 0; x -- )
	{
		COLORREF t_dCol = _PP( o, x, y );

		if ( t_dCol )
		{
			CS_COLORINFO *t_dCur;

			for ( t_dCur = cI; ; t_dCur ++ )
			{
				if ( ! t_dCur->rgb || ( t_dCur->rgb == t_dCol ) )
				{
					t_dCur->rgb = t_dCol;
					t_dCur->cnt ++;
					t_dCur->pnt.x += x;
					t_dCur->pnt.y += y;

					break;
				}
			}
		}
	}

	for ( y =     0; y < CS_ORIGIN_MAX_COLORS - 1; y ++ ) if   ( cI[ y ].rgb != CS_WHITE )
	for ( x = y + 1; x < CS_ORIGIN_MAX_COLORS;     x ++ ) if ( ( cI[ x ].rgb == CS_WHITE ) || ( cI[ x ].cnt > cI[ y ].cnt ) )
	{
		CS_COLORINFO cI_swap;

		memcpy( &cI_swap, &cI[ y ], sizeof( CS_COLORINFO ) );
		memcpy( &cI[ y ], &cI[ x ], sizeof( CS_COLORINFO ) );
		memcpy( &cI[ x ], &cI_swap, sizeof( CS_COLORINFO ) );
	}

	if ( cI[ 0 ].rgb != CS_WHITE )
	{
		return ERROR_CURSORSHOP( 1 );
	}

	for ( x = 0; x < CS_ORIGIN_MAX_COLORS; x ++ )
	{
		cI[ x ].pnt.x = ( cI[ x ].pnt.x / cI[ x ].cnt + 0.5f ) / o _X;
		cI[ x ].pnt.y = ( cI[ x ].pnt.y / cI[ x ].cnt + 0.5f ) / o _Y;
	}

	return ERROR_SUCCESS;
}

const BYTE plasmaIndexes[] =
{
	0, 1, 2, 1, 4, 1,
	2, 1, 2, 3, 2, 5,
	0, 3, 2, 3, 4, 3,
	0, 1, 0, 3, 0, 5,

	0, 1, 4, 5, 0, 0,
	4, 1, 2, 5, 0, 0,
	4, 5, 2, 3, 0, 0,
	0, 5, 4, 3, 0, 0
};

void PlasmaCS_Frame( CS_FRAME *p, int grad, int *nr )
{
	int i, r, e, kr[ 8 * 6 ];

	COLORREF *iPx;

	if ( !nr )
	{
		int x = p _X - 1;
		int y = p _Y - 1;
		{
			int xy[] = { 0, 0, x, y, x >> 1, y >> 1 };

			_PP( p, 0, 0 ) = 1 + ( rand() % ( CS_PLASMA_MAX_COLORS - 1 ) );
			_PP( p, x, 0 ) = 1 + ( rand() % ( CS_PLASMA_MAX_COLORS - 1 ) );
			_PP( p, x, y ) = 1 + ( rand() % ( CS_PLASMA_MAX_COLORS - 1 ) );
			_PP( p, 0, y ) = 1 + ( rand() % ( CS_PLASMA_MAX_COLORS - 1 ) );

			PlasmaCS_Frame( p, grad, xy );

			return;
		}
	}

	iPx = &_PP( p, nr[ 4 ], nr[ 5 ] );

	r = *iPx ? 4 : 0;
	e = ( nr[ 2 ] - nr[ 0 ] < 3 ) && ( nr[ 3 ] - nr[ 1 ] < 3 ) ? 4 : 8;

	for ( i = r * 6; i < e * 6; i ++ ) kr[ i ] = nr[ plasmaIndexes[ i ] ];

	for ( i = 0; r < e; r ++ )
	{
		int *cr = &kr[ r * 6 ];

		if ( r < 4 )
		{
			COLORREF *vPx = &_PP( p, cr[ 4 ], cr[ 5 ] );

			if ( !*vPx )
			{
				int col = ( _PP( p, cr[ 0 ], cr[ 1 ] ) + _PP( p, cr[ 2 ], cr[ 3 ] ) + 1 ) >> 1;

				if ( grad )
				{
					int gen = rand();
					int out = ( gen >> 1 ) % grad;
					col += gen & 1 ? out : -out;
				}

				if ( col < 1 ) col = 1; else if ( col > CS_PLASMA_MAX_COLORS - 1 ) col = CS_PLASMA_MAX_COLORS - 1;

			   *vPx = col;
			}

			i += *vPx;

			if ( r == 3 ) *iPx = ( i + 2 ) >> 2;
		}

		else if ( ( cr[ 2 ] - cr[ 0 ] > 1 ) || ( cr[ 3 ] - cr[ 1 ] > 1 ) )
		{
			cr[ 4 ] = ( cr[ 0 ] + cr[ 2 ] ) >> 1;
			cr[ 5 ] = ( cr[ 1 ] + cr[ 3 ] ) >> 1;

			PlasmaCS_Frame( p, grad >> 1, cr );
		}
	}
}

BOOL MakeCS_Plasma( CS_FACTORY *f )
{
	CS_FRAME *o = &f->orig;
	CS_FRAME *p = &f->plasma;

	COLORREF palette[ CS_PLASMA_MAX_COLORS ];

	int dX = f->head.destSize.cx * f->head.aaFactor;
	int dY = f->head.destSize.cy * f->head.aaFactor;

	int iMake = MakeCS_Frame( p, dX, dY, 1 );

	int x, y;

	for ( x = 0; x < CS_PLASMA_MAX_COLORS; x ++ )
	{
		for ( y = 0; y < 3; y ++ )
		{
			float perc = (float) x / ( CS_PLASMA_MAX_COLORS - 1 );

			float grad = (float) ( (BYTE *) &f->cols[ 1 ].rgb )[ y ] * perc +
						 (float) ( (BYTE *) &f->cols[ 2 ].rgb )[ y ] * ( 1 - perc );

			if ( grad <   0.0f ) grad =   0.0f; else
			if ( grad > 255.0f ) grad = 255.0f;

			( (BYTE * ) &palette[ x ] )[ y ] = (BYTE) grad;
		}
	}

	srand( f->head.randSeed );
	PlasmaCS_Frame( p, (int) ( f->head.gradChaos * 1000 ), 0 );

	for ( y = dY - 1; y >= 0; y -- )
	for ( x = dX - 1; x >= 0; x -- )
	{
		COLORREF *pPx = &_PP( p, x, y );

		int oX = x * o _X / dX;
		int oY = y * o _Y / dY;

		COLORREF oCol = _PP( o, oX, oY );

		if ( oCol == CS_WHITE ) oCol = CS_BLACK;
		if ( oCol != CS_BLACK ) oCol = palette[ *pPx ];

		*pPx = oCol;
	}

	return ERROR_SUCCESS;
}

BOOL RenderCS_Frame( CS_FACTORY *f, WORD rotStep )
{
	CS_FRAME l, w;

	CS_FRAME *o = &f->plasma;
	CS_FRAME *r = &f->rots[ rotStep ];

	const BYTE aaF = f->head.aaFactor;

	const int dX = f->head.destSize.cx * aaF - 1;
	const int dY = f->head.destSize.cy * aaF - 1;

	const float fi = rotStep * (float) M_PI * 2 / f->head.rotSteps;

	const float stepHX = cosf( fi );
	const float stepHY = cosf( M_PI_2 - fi );

	const float stepVX = - stepHY / CS_ASPECT_CORRECTION;
	const float stepVY =   stepHX / CS_ASPECT_CORRECTION;

	const float x1 = dX * stepHX;
	const float y1 = dX * stepHY;
	const float x2 = dY * stepVX;
	const float y2 = dY * stepVY;

	RECT rBounds = { LONG_MAX, LONG_MAX, 0, 0 };

	const float minX = min( min( 0, x1 ), min( x2, x1 + x2 ) );
	const float minY = min( min( 0, y1 ), min( y2, y1 + y2 ) );
	const float maxX = max( max( 0, x1 ), max( x2, x1 + x2 ) );
	const float maxY = max( max( 0, y1 ), max( y2, y1 + y2 ) );

	const float scaleV = stepVX / stepVY;
	const float scaleH = stepHY / stepHX;

	const float aspectX = o _X / ( dX * ( stepHX - stepHY * scaleV ) );
	const float aspectY = o _Y / ( dY * ( stepVY - stepVX * scaleH ) );

	SIZE size = { (long) ( maxX - minX + 1 ), (long) ( maxY - minY + 1 ) };

	int x, y, iMake, sA;

	const int sB = (int) ( f->head.borderWidth * aaF + 0.5f );
	const int sS = (int) ( f->head.shadowDist  * aaF + 0.5f );

//	outtyText( "Creating", rotStep );
//	outtyText( "FLAGGYIE", r->flag );

	iMake = MakeCS_Frame( &l, size.cx, size.cy, 1 );

	if ( iMake ) return iMake;

	for ( y = size.cy; y >= 0; y -- )
	for ( x = size.cx; x >= 0; x -- )
	{
		float oX = ( ( x + minX ) - ( y + minY ) * scaleV ) * aspectX;
		float oY = ( ( y + minY ) - ( x + minX ) * scaleH ) * aspectY;

		if ( ( oX >= 0 ) && ( oX < o _X ) && ( oY >= 0 ) && ( oY < o _Y ) )
		{
			COLORREF oCol = _PP( o, (int) oX, (int) oY );

			if ( oCol != CS_BLACK ) _PD( l, x, y ) = oCol | CS_SHADOW_OPAQ;
		}
	}

	sA = ( sB << 1 ) + sS + aaF;

	iMake = MakeCS_Frame( &w, size.cx + sA, size.cy + sA, 1 );

	if ( iMake )
	{
		KillCS_Frame( &l );
		return iMake;
	}

	for ( y = 2 * sB; y >= 0; y -- )
	for ( x = 2 * sB; x >= 0; x -- )
	{
		if ( ( x - sB ) * ( x - sB ) + ( y - sB ) * ( y - sB ) <= sB * sB )
		{
			BitBlt( w.dci.hDC, x, y, l _x, l _y, l.dci.hDC, 0, 0, SRCPAINT );
		}
	}

	sA = sS * ( 1 + w _x );

	for ( y = w _y - sS - 1; y >= 0; y -- )
	for ( x = w _x - sS - 1; x >= 0; x -- )
	{
		COLORREF *bPx = &_PD( w, x, y );

		if ( *bPx )
		{
			COLORREF *sPx = bPx + sA;

			*bPx = CS_SHADOW_OPAQ;

			if ( rBounds.left   > x ) rBounds.left   = x;
			if ( rBounds.top    > y ) rBounds.top    = y;
			if ( rBounds.right  < x ) rBounds.right  = x;
			if ( rBounds.bottom < y ) rBounds.bottom = y;

			if ( ! *sPx )
			{
				*sPx = CS_SHADOW_LITE;

				if ( rBounds.right  < x + sS ) rBounds.right  = x + sS;
				if ( rBounds.bottom < y + sS ) rBounds.bottom = y + sS;
			}
		}
	}

	for ( y = l _y - 1; y >= 0; y -- )
	for ( x = l _x - 1; x >= 0; x -- )
	{
		COLORREF *nPx = &_PD( l, x, y );

		if ( *nPx )
		{
			COLORREF *bPx = &_PD( w, x + sB, y + sB );

			*bPx = *nPx;

			if ( bPx[ sA ] == CS_SHADOW_LITE )
			{
				bPx[ sA ] = CS_SHADOW_DARK;
			}
		}
	}

	KillCS_Frame( &l );

	size.cx = ( rBounds.right - rBounds.left + 1 ) / aaF;
	size.cy = ( rBounds.bottom - rBounds.top + 1 ) / aaF;

	iMake = MakeCS_Frame( r, size.cx, size.cy, 1 );

	if ( iMake )
	{
		KillCS_Frame( &w );
		return iMake;
	}

	r->fpi.pCenter.x = (long) ( ( f->cols[ 0 ].pnt.x * x1 + f->cols[ 0 ].pnt.y * x2 - minX + sB - rBounds.left ) / aaF );
	r->fpi.pCenter.y = (long) ( ( f->cols[ 0 ].pnt.x * y1 + f->cols[ 0 ].pnt.y * y2 - minY + sB - rBounds.top  ) / aaF );

	for ( y = size.cy - 1; y >= 0; y -- )
	for ( x = size.cx - 1; x >= 0; x -- )
	{
		BYTE a, b, c;
		DWORD argb[ 4 ] = { 0 };

		DWORD *dSeek = &_PD( w, x * aaF + rBounds.left, y * aaF + rBounds.top );

		for ( b = 0; b < aaF; b ++ )
		for ( a = 0; a < aaF; a ++ )
		{
			BYTE *bSeek = (BYTE *) &dSeek[ a + b * w _x ];

			if ( * (DWORD *) bSeek )
			{
				for ( c = 0; c < 4; c ++ )
				{
					argb[ c ] += bSeek[ c ];
				}
			}
		}

		if ( argb[ 3 ] )
		{
			BYTE *bSeek = (BYTE *) &_PP( r, x, y );

			for ( c = 0; c < 4; c ++ )
			{
				bSeek[ c ] = (BYTE) ( argb[ c ] / ( aaF * aaF ) );
			}
		}
	}

	KillCS_Frame( &w );
//	outtyText( "Done", rotStep );

	return ERROR_SUCCESS;
}

CS_FACTORY *MakeCS_Factory( CS_HEADER *h )
{
	int iMake;
	CS_FACTORY *f;

	CS_FRAME g;
	g.bmi.hBM = (HBITMAP) LoadImage( 0, h->resId, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );

	if ( !GetObject( g.bmi.hBM, sizeof( BITMAP ), &g.bmi.bih ) || RaiseCS_Frame( &g ) )
	{
		DeleteObject( g.bmi.hBM );
		return 0;
	}

	g.bmi.bih.biHeight = -g.bmi.bih.biHeight;

	f = VirtualAlloc( 0, sizeof( CS_FACTORY ) + sizeof( CS_FRAME ) * h->rotSteps, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE );

	if ( !f )
	{
		KillCS_Frame( &g );
		return 0;
	}

	memcpy( &f->head, h, sizeof( CS_HEADER ) );

	iMake = MakeCS_Frame( &f->orig, g _x, g _y, 1 );

	if ( !iMake )
	{
		iMake = !BitBlt( f->orig.dci.hDC, 0, 0, g _x, g _y, g.dci.hDC, 0, 0, SRCCOPY );
	}

	KillCS_Frame( &g );

	if ( iMake || EnumCS_ColorInfo( &f->orig, f->cols ) || MakeCS_Plasma( f ) )
	{
		KillCS_Factory( &f );
		return 0;
	}

	for ( iMake = 0; iMake < f->head.rotSteps; iMake ++ )
	{
		f->rots[ iMake ].flag = CS_FLAG_IDLE;
	}

	f->mutex = CreateMutex( 0, 0, 0 );

	return f;
}
