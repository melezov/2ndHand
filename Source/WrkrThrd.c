#include "WrkrThrd.h"
/*
DWORD WINAPI ThrdIS_SYNCHRO( IS_FACTORY *f )
{
    while( 1 )
    {
        Sleep( 100 );
    }

    return ERROR_SUCCESS;
}

IS_SYNCHRO *MakeIS_SYNCHRO( struct IS_FACTORY *f )
{
    DWORD dErr = ERROR_WRKRTHRD( 0x00 );
	IS_SYNCHRO *s;

    DWORD aeou = sizeof( IS_SYNCHRO );

    CHECK_ERROR( s = (IS_SYNCHRO *) MemAlloc( sizeof( IS_SYNCHRO ) / 4 + ( sizeof( IS_FRAME ) / 4 ) * f->head.buffSize ) );

    dErr = ERROR_SUCCESS;

CatchError:

    if ( dErr )
    {
        KillIS_SYNCHRO( s );
        s = 0;
    }

    return s;
}

void KillIS_SYNCHRO( IS_SYNCHRO *s )
{
    if ( s )
    {
        MemFree( s );
    }
}

/*

    CHECK_ERROR( f->drtv = CreateThread( 0, 0, (LPTHREAD_START_ROUTINE) WorkerThread, f, 0, 0 ) );

BOOL PostIS_FRAME( IS_FACTORY *f, POINT *i_pNew )
{
    DWORD iErr = ERROR_WRKRTHRD( 0x00 );

    ot( gonna post: );

    ot( posting... );

    CHECK_ERROR( SetEvent( f->sema ) );

    ot( posted! );

    return ERROR_SUCCESS;

    REPORT_ERROR;

    return iErr;
}

DWORD WINAPI WorkerThread( IS_FACTORY *f )
{
    while( 1 )
    {
        DWORD iErr = ERROR_WRKRTHRD( 0x10 );

        ot( gonna while: );

        CHECK_ERROR( WaitForSingleObject( f->sema, INFINITE ) == WAIT_OBJECT_0 );

        ot( whiling... );
        Sleep( 100 );

        ot( whiled! );

        continue;

        REPORT_ERROR;

        break;
    }

    ot( Thread Exiting );

    return 0;
}
*/
