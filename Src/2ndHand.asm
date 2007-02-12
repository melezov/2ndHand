; ###################################################################################################################################################

     DATA

     NOTIFYICONDATAA:
     ;{
          NID_cbSize     dd   58h ; sizeof( NOTIFYICONDATAA )
          NID_hwnd       dd   0h ; ---
          NID_uID        dd   0h
          NID_uFlags     dd   7h ; NIF_MESSAGE | NIF_ICON | NIF_TIP
          NID_uCall      dd   400h ; WM_USER
          NID_hIcon      dd   0h ; ---

          SZ_AN          db   "2nd Hand v0.1a", 0h ; szTip [ 40h ]
          SZ_CN          db   "2nd_HAND_CLASS", 0h
          SZ_TRAY        db   "TaskbarCreated", 0h
          SZ_EXIT        db   "Exit", 0h

          NID_PADDING    db   40h - ( NID_PADDING - SZ_AN ) dup 0FDh
     ;}

     WNDCLASSEXA:
     ;{
          WCX_cbSize     dd   30h ; sizeof( WNDCLASSEXA )
          WCX_style      dd   0h
          WCX_lpfnProc   dd   addr SndHandProc
          WCX_cbClsExtra dd   0h
          WCX_cbWndExtra dd   0h
          WCX_hInstance  dd   0h ; ---
          WCX_hIcon      dd   0h ; ---
          WCX_hCursor    dd   0h
          WCX_hbrBack    dd   10h
          WCX_lpszMenu   dd   0h
          WCX_lpszClass  dd   addr SZ_CN
          WCX_hIconSm    dd   0h
     ;}

     MSG            dd  7h dup ?
     PAINTSTRUCT    dd 10h dup ?
     ICONINFO       dd  5h dup ?

     g_dTray   dd ?
     g_hDC     dd ?

; ###################################################################################################################################################

     CODE

; ###################################################################################################################################################

     SndHandProc: frame w_hWnd, w_dMsg, w_wParam, w_lParam
     ;{
          mov       eax, [ w_dMsg ]

.ShpPai:  cmp       eax, 0Fh ; WM_PAINT
          jnz       >.ShpUsr
          ;{
               invoke    BeginPaint, [ w_hWnd ], addr PAINTSTRUCT
               invoke    BitBlt, eax, 0, 0, 20h, 20h, [ g_hDC ], 0, 0, 0CC0020h ; SRCCOPY
               invoke    EndPaint, [ w_hWnd ], addr PAINTSTRUCT
               jmp       >.ShpXor
          ;}

.ShpUsr:  cmp       eax, 400h ; WM_USER
          jnz       >.ShpDst
          ;{
               mov       ecx, [ w_lParam ]
               cmp       ecx, 204h ; WM_RBUTTONDOWN
               jnz       >.ShpXor
               ;{
                    invoke    CreatePopupMenu
                    push      ebx
                    xchg      eax, ebx
                    ;{
                         invoke    AppendMenuA, ebx, 0h, 9h, addr SZ_EXIT ; MF_BYCOMMAND or MF_ENABLED or MF_STRING, IDM_EXIT
                         invoke    SetForegroundWindow, [ w_hWnd ]

                         invoke    GetCursorPos, esp, esp, esp
                         pop       ecx, edx
                         invoke    TrackPopupMenuEx, ebx, 41EAh, ecx, edx, [ w_hWnd ], 0h ; TPM_RIGHTBUTTON or TPM_RIGHTALIGN or TPM_BOTTOMALIGN or TPM_VERTICAL or TPM_NONOTIFY or TPM_RETURNCMD or TPM_NOANIMATION

                         cmp       eax, 9h
                         jnz       >
                         ;{
                              invoke    DestroyWindow, [ w_hWnd ]
                         ;}
:
                         invoke    DestroyMenu, ebx
                         pop       ebx
                    ;}
               ;}

.ShpXor:       xor       eax, eax
               ret
          ;}

.ShpDst:  cmp       eax, 2h ; WM_DESTROY
          jnz       >.ShpDef
          ;{
               invoke    Shell_NotifyIconA, 2h, addr NOTIFYICONDATAA ; NIM_DELETE
               invoke    PostQuitMessage, 0h

               jmp       <.ShpXor
          ;}

.ShpDef:  cmp       eax, [ g_dTray ]
          jnz       >
          ;{
               invoke    Shell_NotifyIconA, 0h, addr NOTIFYICONDATAA ; NIM_ADD

               jmp       <.ShpXor
          ;}

:         invoke    DefWindowProcA, [ w_hWnd ], eax, [ w_wParam ], [ w_lParam ]
          ret
     ;}
     endf

; ###################################################################################################################################################

     2ndHandEntry:
     ;{
          invoke    GetModuleHandleA, 0h
          mov       [ WCX_hInstance ], eax

          invoke    LoadIconA, eax, 10h
          mov       [ WCX_hIcon ], eax
          mov       [ NID_hIcon ], eax

          invoke    GetIconInfo, eax, addr ICONINFO

          invoke    RegisterClassExA, addr WNDCLASSEXA
          or        ax, ax
          jnz       >

.SndE:    xor       eax, eax
          inc       eax
          ret

:         xor       eax, eax
          invoke    CreateWindowExA, 80h, addr SZ_CN, addr SZ_AN, 80000000h, eax, eax, eax, eax, eax, eax, eax, eax ; WS_EX_TOOLWINDOW, WS_POPUP
          or        eax, eax
          jz        <.SndE

          mov       [ NID_hwnd ], eax
          invoke    RatHookInit, eax

          invoke    CreateCompatibleDC, 0h
          mov       [ g_hDC ], eax
          invoke    SelectObject, eax, [ ICONINFO + 0Ch ]

          pushad
          ;{
               xor       eax, eax
               mov       ebp, [ CreateRectRgn ]
               invoke    ebp, eax, eax, eax, eax
               xchg      eax, ebx

               xor       esi, esi
.RG_Y:         xor       edi, edi
.RG_X:         invoke    GetPixel, [ g_hDC ], edi, esi
               or        eax, eax
               jnz       >

               inc       esi
               push      esi
               dec       esi
               inc       edi
               push      edi
               dec       edi

               invoke    ebp, edi, esi
               invoke    CombineRgn, ebx, ebx, eax, 2h ; RGN_OR

:              inc       edi
               cmp       edi, 20h
               jnz       <.RG_X
               inc       esi
               cmp       esi, 20h
               jnz       <.RG_Y

               invoke    SetWindowRgn, [ NID_hwnd ], ebx, 0h
               invoke    SelectObject, [ g_hDC ], [ ICONINFO + 10h ]
          ;}
          popad


          invoke    RegisterWindowMessageA, addr SZ_TRAY
          mov       [ g_dTray ], eax

          invoke    SendMessageA, [ NID_hwnd ], eax, eax, eax

.SndP:    ;{
               xor       eax, eax
               invoke    GetMessageA, addr MSG, eax, eax, eax
               inc       eax
               jz        <.SndP
               dec       eax
               jz        >.SndX

               invoke    DispatchMessageA, addr MSG
               jmp       <.SndP
          ;}

          invoke    RatHookFree

.SndX:    xor       eax, eax
          ret
     ;}

; ###################################################################################################################################################
