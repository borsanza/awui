@echo off

setlocal

cd build

rem C:\msys64\mingw64\bin\mingw32-make.exe clean

del CMakeCache.txt 2>nul
if exist build.ninja ninja -t clean
del build.ninja 2>nul

del libawui\*.dll 2> nul
del libawui\*.dll.a 2> nul
del samples\awSlider\*.dll 2> nul
del samples\awTest\*.dll 2> nul
del samples\awuiDemo\*.dll 2> nul
del samples\stationTV\*.dll 2> nul

endlocal
