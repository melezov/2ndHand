#include "CursorShop.h"

void KillCS_Factory( CS_FACTORY *f )
{
    if ( f )
    {
        KillCS_Frame( f->rndy );
        MemFree( f );
    }
}

CS_FACTORY *MakeCS_Factory( CS_HEADER *h )
{
	BYTE iMake = 0xF0;
    CS_FRAME *o = 0;

    CS_FACTORY *f = (CS_FACTORY *) MemAlloc( sizeof( CS_FACTORY ) / 4 );
    if ( !f ) goto MakeError; iMake ++;

    MemZero( sizeof( CS_FACTORY ) / 4, f );
    MemCopy( sizeof( CS_HEADER ) / 4, &f->head, h );

    o = LoadCS_Frame( h->resId );
    if ( !o ) goto MakeError; iMake ++;

    f->rndy = PlsmCS_Frame( o, h );
    if ( !f->rndy ) goto MakeError; iMake ++;

    return f;

MakeError:

    MessageBox( 0, TEXT( "MakeCS_Factory crash!" ), 0, MB_ICONHAND );

    KillCS_Frame( o );
    KillCS_Factory( f );

    SetLastError( ERROR_CURSORSHOP( iMake ) );
    return 0;
}
