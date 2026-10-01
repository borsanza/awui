# Compilación para Windows (64 bits) desde Linux con MinGW-w64. Lo usa el preset "windows", dentro del contenedor
# de scripts/windows (Fedora, que trae SDL2, cairo, pango y GLEW compilados para MinGW)

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(MINGW_TRIPLET x86_64-w64-mingw32)
set(CMAKE_C_COMPILER ${MINGW_TRIPLET}-gcc)
set(CMAKE_CXX_COMPILER ${MINGW_TRIPLET}-g++)
set(CMAKE_RC_COMPILER ${MINGW_TRIPLET}-windres)

# Donde están las librerías de Windows (en Fedora, /usr/x86_64-w64-mingw32/sys-root/mingw)
if(NOT MINGW_SYSROOT)
	set(MINGW_SYSROOT /usr/${MINGW_TRIPLET}/sys-root/mingw)
endif()
set(CMAKE_FIND_ROOT_PATH ${MINGW_SYSROOT})

# Los programas (compilador, objdump) son los del sistema; las librerías y cabeceras, solo las de Windows
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# pkg-config de las librerías de Windows
set(PKG_CONFIG_EXECUTABLE ${MINGW_TRIPLET}-pkg-config)
