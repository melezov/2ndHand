; #############################################################################################################################

    #define UNICODE 1
    STRINGS UNICODE

    #include "..\Source\RatHook.h"

; #############################################################################################################################

    DATA

    gx_vMem     dd 0

    gr_hInst    dd ?
    gx_hWnd     dd ?
    gr_hRat     dd ?

    SZ_BUFF     db 200 dup ?

; #############################################################################################################################

    CODE

; #############################################################################################################################

    EXPORT:1:NONAME ; void MemZero( DWORD s_dSize, void *s_vDest )

    MemZero:
    ;{
        push    edi
        mov     edi, edx
        xor     eax, eax

        cld
        rep     stosd

        pop     edi
        ret
    ;}

; #############################################################################################################################

    EXPORT:2:NONAME ; void MemCopy( DWORD s_dSize, void *s_vDest, void *s_vSrc )

    MemCopy:
    ;{
        push    edi, esi
        mov     edi, edx
        mov     esi, [ esp + 0Ch ]

        cld
        rep     movsd

        pop     esi, edi
        ret     4
    ;}

; #############################################################################################################################

    EXPORT:3:NONAME ; void MessagePump()

    MessagePump:
    ;{
        sub     esp, 1Ch
        mov     ebp, esp

        xor     ebx, ebx
.MP_L:  invoke  GetMessage, ebp, ebx, ebx, ebx

        inc     eax
        jz      <.MP_L

        dec     eax
        jz      >.MP_Q

        invoke  TranslateMessage, ebp
        invoke  DispatchMessage, ebp

        jmp     <.MP_L

.MP_Q:  invoke  ExitProcess, [ ebp + 8h ]
    ;}

; #############################################################################################################################

    EXPORT:4:NONAME ; HWND RatHookAction( HWND s_hWnd )

    RatHookAction:
    ;{
        push    ebp
        lea     ebp, [ gr_hRat ]

        or      ecx, ecx
        jz      >.RHA_Z
        ;{
            cmp     D[ ebp ], 0h
            jnz     >.RHA_S
            ;{
                push    ecx
                invoke  SetWindowsHookEx, 0Eh, addr RatHookProc, [ gr_hInst ], 0h
                pop     ecx
                or      eax, eax
                jz      >.RHA_Q

                mov     [ ebp ], eax
            ;}

.RHA_S:     mov     eax, ecx
.RHA_E:     mov     [ gx_hWnd ], ecx
.RHA_Q:     pop     ebp
            ret
        ;}

.RHA_Z: mov     eax, [ ebp ]
        or      eax, eax
        jz      <.RHA_Q
        ;{
            invoke  UnhookWindowsHookEx, eax
            or      eax, eax
            jz      <.RHA_Q
            ;{
                mov     eax, [ gx_hWnd ]
                xor     ecx, ecx
                mov     [ ebp ], ecx
                jmp     <.RHA_E
            ;}
        ;}
    ;}

; #############################################################################################################################

    RatHookEntryPoint: frame r_hInst, r_dReason, r_vRsr
    ;{
        mov     eax, [ r_dReason ]
        dec     eax
        jnz     >.RHE_S
        ;{
            push    [ r_hInst ]
            pop     [ gr_hInst ]
            jmp     >.RHE_Q
        ;}

.RHE_S: inc     eax
        jnz     >.RHE_Q
        ;{
            xor     ecx, ecx
            call    RatHookAction
        ;}

.RHE_Q: xor     eax, eax
        inc     eax
        ret
    ;}
    endf

; #############################################################################################################################

    RatHookProc: frame r_nCode, r_wParam, r_lParam
    ;{
        invoke    CallNextHookEx, [ gr_hRat ], [ r_nCode ], [ r_wParam ], [ r_lParam ]

        mov       eax, [ r_lParam ]
        invoke    wsprintf, addr SZ_BUFF, "[ Msg : %08Xh | P1os : ( %04d, %04d ) | Data : %08Xh | Time : %08Xh ]       ", [ r_wParam ], [ eax ], [ eax + 4 ], [ eax + 8 ], [ eax + 10h ]
        add       esp, 1Ch

        invoke    GetDC, 0
        push      eax
        invoke    TextOut, eax, 50, 50, addr SZ_BUFF, 54h
        invoke    ReleaseDC, 0h

        invoke    PostMessage, [ gx_hWnd ], 401h, 0, 0 ; WM_RATHOOK

        xor       eax, eax
        ret
     ;}
     endf

; #############################################################################################################################

    EXPORT:5:NONAME ; DWORD NRandom( DWORD *s_dRNG )

    NRandom:
    ;{
        push    ecx
        mov     eax, [ ecx ]

        test    eax, 80000000h
        jz      >.NR_S
        add     eax, 7fffffffh
.NR_S:  xor     edx, edx
        mov     ecx, 127773
        div     ecx
        mov     ecx, eax
        mov     eax, 16807
        mul     edx
        mov     edx, ecx
        mov     ecx, eax
        mov     eax, 2836
        mul     edx
        sub     ecx, eax

        pop     edx
        mov     [ edx ], ecx
        mov     eax, ecx
        ret
    ;}

; #############################################################################################################################

    EXPORT:6:NONAME ; DWORD *MemAlloc( DWORD s_dSize )

    MemAlloc:
    ;{
        mov     eax, [ gx_vMem ]
        or      eax, eax
        jz      >.MA_N
        ;{
            shl     ecx, 2
            push    ecx
            push    eax
            mov     eax, [ eax ]
            call    D[ eax + 12 ]
            ret
        ;}

.MA_N:  push    ecx
        invoke  CoGetMalloc, 1, ADDR gx_vMem
        or      eax, eax
        pop     ecx
        jz      MemAlloc

        xor     eax, eax
        ret
    ;}

; #############################################################################################################################

    EXPORT:7:NONAME ; void MemFree( DWORD *s_vDest )

    MemFree:
    ;{
        push    ecx
        mov     eax, [ gx_vMem ]
        push    eax
        mov     eax, [ eax ]
        call    D[ eax + 20 ]
        ret
    ;}

; #############################################################################################################################
