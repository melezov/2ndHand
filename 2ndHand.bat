@echo off

attrib /s -h

rmdir /q /s Release
move Solution\Release

Tools\UPX --brute Release\2ndHand.exe
Tools\UPX --brute Release\RatHook.dll

rmdir /q /s "Visual Studio 2005"
rmdir /q /s "Solution\Visual Studio 2005"
rmdir /q /s "Debug\Visual Studio 2005"
rmdir /q /s Solution\Debug

del /s *.rar
del /s *.aps
del /s *.dep
del /s *.exp
del /s *.htm
del /s *.idb
del /s *.ilk
del /s *.lib
del /s *.manifest
del /s *.ncb
del /s *.obj
del /s *.pdb
del /s *.res
del /s *.suo
del /s *.user

cd ..
2ndHand\tools\rar a -m5 2ndHand\2ndHand 2ndHand