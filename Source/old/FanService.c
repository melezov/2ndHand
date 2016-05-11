#include "CursorShop.h"

DWORD sema = 1;
DWORD tesy = 0x0;
DWORD hehe = 0;

HANDLE mutex;

DWORD WINAPI WorkerThread()
{
    int i;

    for ( i = 0; i < 0x111111; i ++ )
    {
        __asm
        {
           lea eax, [ tesy ]
           mov  edx, 1

           lock xadd DWORD PTR [ eax ], edx
        }

//        WaitForSingleObject( mutex, INFINITE );
/*        __asm
        {
            xor     eax, eax     // ; set AX to zero
        L1:
            lock    xchg eax, [ sema ]  //  ; atomic test and set. sema = 0, ax = sema
            or      eax, eax
            jz      L1

            mov eax, [ tesy ]
            inc eax
            mov [ tesy ], eax

            xor     eax, eax
            inc     eax
            lock    xchg eax, [ sema ]
        }
//        ReleaseMutex( mutex );

/*
            xor     eax, eax     // ; set AX to zero
            inc     eax
        L1:
            lock  xadd eax xchg    eax, [ sema ]  //  ; atomic test and set. sema = 0, ax = sema
            or      eax, eax      // ; is the value we obtained from sema 0?
            jz      L1            // ;  yes, wait for someone to release semaphore
            dec     eax            // ; no, we got the semaphore.
            lock xchg    eax, [ sema ]    //; decrease by one and store

            mov eax, [ tesy ]
            inc eax
            mov [ tesy ], eax

            lock xchg    eax, [ sema ] //    ; sema value may have been changed by other threads
            inc          eax           //   ; bump to notify we have released it
            lock xchg    eax, sema      //; atomic store
        }


*/

    }

    hehe ++;

    return 0;
}

DWORD MainGxUI()
{
    mutex = CreateMutex( 0, 0, 0 );

    CreateThread( 0, 0, (LPTHREAD_START_ROUTINE) WorkerThread, 0, 0, 0 );
    WorkerThread();

    while ( hehe < 2 ) ;
    return 0;


float abz( float x )
{
    __asm and x, 7FFFFFFFh

    return x;
}

CS_FRAME *InitCS_Frame( CS_FACTORY *r, POINT *pOld, POINT *pNew )
{
    float newAngle;

    int delX = pOld->x - pNew->x;
    int delY = pOld->y - pNew->y;

    if ( !delX && !delY ) return (CS_FRAME *) -1;

    __asm
    {
        finit

        push    delX
        push    delY
        fild     [ esp ]
        fild     [ esp + 4 ]
        fpatan

        fstp    [ esp + 4 ]
        fwait
        pop     eax
        pop     newAngle
    }

//    while( abz( oldAngle - newAngle - pi2 ) < abz( oldAngle - newAngle ) ) oldAngle -= pi2;
//    while( abz( oldAngle - newAngle + pi2 ) < abz( oldAngle - newAngle ) ) oldAngle += pi2;

    return RendCS_Frame( r, newAngle );
}

void ShowCS_Frame( HWND h, CS_SYNCHRO *s )
{
    CS_FRAME *f = s->work;

    BLENDFUNCTION blenda = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
    SIZE  t_pSiz = { f->_x, f->_y };

    POINT t_pSrc = { 0, 0 };
    POINT t_pDst = { s->dest.x - f->_ix, s->dest.y - f->_iy };

    UpdateLayeredWindow( h, 0, &t_pDst, &t_pSiz, f->hDC, &t_pSrc, 0, &blenda, ULW_ALPHA );
    SetWindowPos( h, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW );
}

DWORD WINAPI WorkerThread( CS_FACTORY *f )
{
    while( 1 )
    {
        CS_SYNCHRO *seek = f->reqs, *kill;

        WaitForSingleObject( f->sema, INFINITE );
        WaitForSingleObject( f->muta, INFINITE );
        COUNT +=2;

        if ( !f->swdn )
        {
            while ( seek )
            {
                if ( !seek->work )
                {
                    seek->work = InitCS_Frame( f, &seek->sorc, &seek->dest );

                    if ( seek->work != (CS_FRAME *) -1 )
                    {
                        ShowCS_Frame( f->hwnd, seek );
                        KillCS_Frame( seek->work );
                        seek->work = (CS_FRAME *) -1;
                    }
                }

                seek = seek->next;
            }
        }
        else
        {
            while ( seek )
            {
                kill = seek;
                seek = seek->next;
                MemFree( kill );
            }

            f->reqs = 0;
        }

        COUNT -= 2;
        ReleaseMutex( f->muta );

        if ( f->swdn ) return 0;
    }
}

BOOL DestroyQueue( CS_FACTORY *f )
{
    WaitForSingleObject( f->muta, INFINITE );
    COUNT++;
    f->swdn = 1;
    SetEvent( f->sema );
    COUNT--;
    ReleaseMutex( f->muta );

    return ERROR_SUCCESS;
}

BOOL PostCS_Frame( CS_FACTORY *f, POINT *pNew )
{
    CS_SYNCHRO *n = 0, *seek;
    WaitForSingleObject( f->muta, INFINITE );
    COUNT += 4;

    n = (CS_SYNCHRO *) MemAlloc( sizeof( CS_SYNCHRO ) / 4 );
    MemZero( sizeof( CS_SYNCHRO ) / 4, n );

    n->dest.x = pNew->x;
    n->dest.y = pNew->y;

    n->next = 0;

    if ( seek = f->reqs ) while ( seek->next ) seek = seek->next;
    if ( seek ) seek->next = n; else seek = f->reqs = n;

    n->sorc.x = seek->dest.x;
    n->sorc.y = seek->dest.y;

    SetEvent( f->sema );
    COUNT -= 4;
    ReleaseMutex( f->muta );

    return ERROR_SUCCESS;
}
