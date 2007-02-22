#include "CursorFactory.h"
/*
#define MAX_HDC_DEPTH 10

HDC		s_hDCStash[ MAX_HDC_DEPTH ] = { 0 };
HGDIOBJ s_hGOStash[ MAX_HDC_DEPTH ];

HDC MakeDC( HBITMAP s_hBm )
{
	BYTE i = 0;
	for ( ; i < MAX_HDC_DEPTH; i ++ )
	{
		if ( !s_hDCStash[ i ] )
		{
			HDC t_hDC =
			s_hDCStash[ i ] = CreateCompatibleDC( 0 );
			s_hGOStash[ i ] = SelectObject( t_hDC, s_hBm );

			return t_hDC;
		}
	}

	MessageBox( 0, "HDC LEAK!!! <10>", 0, MB_ICONHAND | MB_OK );
	return 0;
}

HBITMAP KillDC( HDC s_hDC )
{
	BYTE i = 0;
	for ( ; i < MAX_HDC_DEPTH; i ++ )
	{
		if ( s_hDCStash[ i ] == s_hDC )
		{
			HGDIOBJ t_hRet = SelectObject( s_hDC, s_hGOStash[ i ] );
			DeleteDC( s_hDC );
			s_hDCStash[ i ] = 0;

			return (HBITMAP) t_hRet;
		}
	}

	MessageBox( 0, "HDC CRASH!!! <10>", 0, MB_ICONHAND | MB_OK );
	return 0;
}

void KillBM( CF_WS &s )
{
	KillDC( s.hDC );
	DeleteObject( s.hBM );
}
*/
