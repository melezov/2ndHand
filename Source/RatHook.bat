@echo off

echo.Step #(1/3) : %~nx1 ASSEMBLY to %~n1.obj
"..\Tools\GoAsm.exe" /b /ni /fo "%~2\%~n1.obj" %1
if errorlevel 1 goto errObj

echo.Step #(2/3) : %~n1.obj LIYNKING to %~n1.dll
"..\Tools\GoLink.exe" /ni /dll /entry RatHookEntryPoint "%~2\%~n1.obj" user32.dll kernel32.dll gdi32.dll ole32.dll
if errorlevel 1 goto errDll

echo.Step #(3/3) : %~n1.def XPORTING to %~n1.lib
"..\Tools\lib.exe" /machine:x86 /def:"%~dpn1.def" /out:"%~2%~n1.lib"
if errorlevel 1 goto errLib

goto Bye

:errObj
echo.
echo.DLL ASSEMBLY Error!
goto Bye

:errDll
echo.
echo.DLL LINKER Error!
goto Bye

:errLib
echo.
echo.DEF EXPORT Error!
goto Bye

:Bye