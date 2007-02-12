@echo off

cd Out
echo.

echo Step #(1/5) RESOURCES
"%GoTools%Gorcjorg\GoRC.exe" /b /ni /r /fo "2ndHand.res" "..\Res\2ndHand.rc"
if errorlevel 1 goto errrc

echo Step #(2/5) DLL ASSEMBLY
"%GoTools%Goasm\GoAsm.exe" /b /ni /fo "RatHook.obj" "..\Src\RatHook.asm"
if errorlevel 1 goto errdllasm

echo Step #(3/5) DLL LINKER
"%GoTools%Golink\GoLink.exe" /ni /dll /entry RatHookEntry  "GDI32.dll" "user32.dll" "RatHook.obj"
if errorlevel 1 goto errdlllink

echo Step #(4/5) EXE ASSEMBLY
"%GoTools%Goasm\GoAsm.exe" /b /ni /fo "2ndHand.obj" "..\Src\2ndHand.asm"
if errorlevel 1 goto errexeasm

echo Step #(5/5) EXE LINKER
"%GoTools%Golink\GoLink.exe" /ni /entry 2ndHandEntry "RatHook.dll" "GDI32.dll" "kernel32.dll" "user32.dll" "shell32.dll" "2ndHand.obj" "2ndHand.res"
if errorlevel 1 goto errexelink

if "%1" == "" goto Bye
echo Executing %1 ...
%1

goto Bye

:errrc
echo.
echo RESOURCES Error!
goto Bye

:errdllasm
echo.
echo DLL ASSEMBLY Error!
goto Bye

:errdlllink
echo.
echo DLL LINKER Error!
goto Bye

:errexeasm
echo.
echo EXE ASSEMBLY Error!
goto Bye

:errexelink
echo.
echo EXE LINKER Error!
goto Bye

:Bye