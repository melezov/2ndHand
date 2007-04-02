#define UNICODE 1
STRINGS UNICODE

#ifdef UNICODE

    #define GetMessage GetMessageW
    #define DispatchMessage DispatchMessageW
    #define SetWindowsHookEx SetWindowsHookExW
    #define wsprintf wsprintfW
    #define PostMessage PostMessageW
    #define TextOut TextOutW

#else

    #define GetMessage GetMessageA
    #define DispatchMessage DispatchMessageA
    #define SetWindowsHookEx SetWindowsHookExA
    #define wsprintf wsprintfA
    #define PostMessage PostMessageA
    #define TextOut TextOutA

#endif
