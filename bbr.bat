@echo off

setlocal

call buildvars.bat

cd build

rem cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ..
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release ..
if %ERRORLEVEL% neq 0 (
    echo Error generando archivos de proyecto con CMake.
    exit /b %ERRORLEVEL%
)

rem C:\msys64\mingw64\bin\mingw32-make.exe -j%jobs% > nul
ninja

if %ERRORLEVEL% neq 0 (
    echo Error compilando el proyecto.
    exit /b %ERRORLEVEL%
)

endlocal
