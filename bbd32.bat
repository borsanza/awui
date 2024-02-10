@echo off

setlocal

call buildvars32.bat

mkdir build32
cd build32

cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_FLAGS="-m32" -DCMAKE_CXX_FLAGS="-m32" ..
if %ERRORLEVEL% neq 0 (
    echo Error generando archivos de proyecto con CMake.
    exit /b %ERRORLEVEL%
)

ninja
if %ERRORLEVEL% neq 0 (
    echo Error compilando el proyecto.
    exit /b %ERRORLEVEL%
)

endlocal
