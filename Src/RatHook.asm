; ###################################################################################################################################################

     DATA

     g_hInst   dd ?
     g_hWnd    dd ?
     g_hRat    dd ?

     SZ_BUFF   db 400h dup ?

; ###################################################################################################################################################

     CODE

; ###################################################################################################################################################

     RatHookProc: frame r_nCode, r_wParam, r_lParam
     ;{
          invoke    CallNextHookEx, [ g_hRat ], [ r_nCode ], [ r_wParam ], [ r_lParam ]

          mov       ecx, [ r_lParam ]
          invoke    SetWindowPos, [ g_hWnd ], -1, [ ecx ], [ ecx + 4 ], 20h, 20h, 40h, ecx

          pop       ecx
          invoke    wsprintfA, addr SZ_BUFF, "[ Msg : %08Xh | Pos : ( %04d, %04d ) | Data : %08Xh | Time : %08Xh ]       ", [ r_wParam ], [ ecx ], [ ecx + 4 ], [ ecx + 8 ], [ ecx + 10h ]
          add       esp, 1Ch

          invoke    GetDC, 0
          push      eax
          invoke    TextOutA, eax, 50, 50, addr SZ_BUFF, 54h
          invoke    ReleaseDC, 0h

          xor       eax, eax
          ret
     ;}
     endf

     EXPORT RatHookInit: frame w_hWnd
     ;{
          push      [ w_hWnd ]
          pop       [ g_hWnd ]

          invoke    SetWindowsHookExA, 0Eh, addr RatHookProc, [ g_hInst ], 0h ; WH_MOUSE
          mov       [ g_hRat ], eax

          ret
     ;}
     endf

     EXPORT RatHookFree:
     ;{
          invoke    UnhookWindowsHookEx, [ g_hRat ]
          ret
     ;}

     RatHookEntry: frame r_hInst, r_dReason, r_vRsr
     ;{
          mov       eax, [ r_dReason ]
          dec       eax
          jnz       >.RHE_X
          ;{
               push      [ r_hInst ]
               pop       [ g_hInst ]
          ;}

.RHE_X:   xor       eax, eax
          inc       eax
          ret
     ;}
     endf


; ###################################################################################################################################################
