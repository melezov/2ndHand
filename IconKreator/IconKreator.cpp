#include <windows.h>

# pragma pack (1)

struct ICONDIR
{
    WORD           idReserved;   // Reserved (must be 0)
    WORD           idType;       // Resource Type (1 for icons)
    WORD           idCount;      // How many images?
};

struct ICONDIRENTRY
{
    BYTE            bWidth;          // Width, in pixels, of the image
    BYTE            bHeight;         // Height, in pixels, of the image
    BYTE            bColorCount;     // Number of colors in image (0 if >=8bpp)
    BYTE            bReserved;       // Reserved ( must be 0)
    WORD            wPlanes;         // Color Planes
    WORD            wBitCount;       // Bits per pixel
    DWORD           dwBytesInRes;    // How many bytes in this resource?
    DWORD           dwImageOffset;   // Where in the file is this image?
};

struct ICON_32x32x4
{
    ICONDIR         icondir;
    ICONDIRENTRY    icondirsingle;

    BITMAPINFOHEADER xBIH;
    RGBQUAD          xRGB[ 17 ];
};

#define FILENAME "Access"

int WINAPI WinMainCRTStartup()
{
    BYTE buff[ 766 ];
	DWORD readed;

	HANDLE inputIcon = CreateFile( "Src\\" FILENAME ".ico", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE, 0 );
    ReadFile( inputIcon, buff, 766, &readed, 0 );
    CloseHandle( inputIcon );

	ICON_32x32x4 * Icon = (ICON_32x32x4 *) buff;

	HBITMAP hBM = (HBITMAP) LoadImage( 0, "Bmp\\" FILENAME ".bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );
    HDC hDC = CreateCompatibleDC( 0 );
    SelectObject( hDC, hBM );

	DWORD cols[ 16 ], colN = 0;

	for ( int y = 0; y < 32; y ++ )
	for ( int x = 0; x < 32; x ++ )
	{
		DWORD cc = GetPixel( hDC, x, 32 - y ) & 0xffffff;

		if ( cc == 0xffffff ) continue;

		DWORD index = -1;
		for ( DWORD c = 0; c < colN; c ++ ) if ( cc == cols[ c ] ) index = c;
		if ( index == -1 ) cols[ colN ++ ] = cc;
	}

	DWORD index = 0x7E;

	for ( int y = 0; y < 32; y ++ )
	for ( int x = 0; x < 32; x += 2 )
	{
		DWORD c0 = GetPixel( hDC, x    , 32 - y ) & 0xffffff;
		DWORD c1 = GetPixel( hDC, x + 1, 32 - y ) & 0xffffff;

		DWORD i0 = -1, i1 = -1;

		for ( DWORD c = 0; c < colN; c ++ )
		{
			if ( c0 == cols[ c ] ) i0 = c;
			if ( c1 == cols[ c ] ) i1 = c;
		}

		if ( c0 == 0xffffff ) i0 = 0;
		if ( c1 == 0xffffff ) i1 = 0;

		BYTE bOut = (BYTE) ( ( ( i0 & 0xf ) << 4 ) + ( i1 & 0xf ) );

		buff[ index ++ ] = bOut;
	}

	for ( int i = 0; i < 16; i ++ )
	{
		Icon->xRGB[ i ].rgbRed   = (BYTE) ( cols[ i ] >>  0 );
		Icon->xRGB[ i ].rgbGreen = (BYTE) ( cols[ i ] >>  8 );
		Icon->xRGB[ i ].rgbBlue  = (BYTE) ( cols[ i ] >> 16 );
		Icon->xRGB[ i ].rgbReserved = 0;
	}

	DeleteDC( hDC );
	DeleteObject( hBM );

    HANDLE outputIcon = CreateFile( "Dst\\" FILENAME ".ico", GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_ARCHIVE, 0 );
    WriteFile( outputIcon, buff, 766, &readed, 0 );
    CloseHandle( outputIcon );

	return 0;
}
