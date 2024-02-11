@echo off

setlocal

cd build

rem C:\msys64\mingw64\bin\mingw32-make.exe clean

del CMakeCache.txt 2>nul
if exist build.ninja ninja -t clean
del build.ninja 2>nul

endlocal
