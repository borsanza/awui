@echo off

setlocal

call buildvars32.bat

cd build

rem cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_FLAGS="-m32" -DCMAKE_CXX_FLAGS="-m32" ..
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_FLAGS="-m32" -DCMAKE_CXX_FLAGS="-m32" ..
if %ERRORLEVEL% neq 0 (
    echo Error generando archivos de proyecto con CMake.
    exit /b %ERRORLEVEL%
)

rem C:\msys64\mingw32\bin\mingw32-make.exe -j%jobs% > nul
ninja

if %ERRORLEVEL% neq 0 (
    echo Error compilando el proyecto.
    exit /b %ERRORLEVEL%
)

endlocal
