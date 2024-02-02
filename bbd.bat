@echo off

setlocal

call buildvars.bat

cd build

cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Debug ..
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
