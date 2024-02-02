@echo off

setlocal

cd build

del CMakeCache.txt 2>nul
if exist build.ninja ninja -t clean
del build.ninja 2>nul

endlocal
