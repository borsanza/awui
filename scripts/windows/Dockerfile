# Entorno para compilar awui para Windows desde Linux (lo usa scripts/build-windows.sh)
FROM fedora:44

RUN dnf -y install \
		cmake ninja-build \
		mingw64-gcc-c++ mingw64-pkg-config \
		mingw64-sdl2-compat mingw64-SDL2_image mingw64-cairo mingw64-pango mingw64-glew \
		mingw32-nsis \
		json-devel \
	&& dnf clean all

# nlohmann_json no está empaquetado para MinGW, pero son solo cabeceras: se copian las de Linux al sysroot de Windows
# (con su configuración de CMake, que calcula la ruta de las cabeceras desde donde está)
RUN SYSROOT=/usr/x86_64-w64-mingw32/sys-root/mingw \
	&& cp -r /usr/include/nlohmann "$SYSROOT/include/" \
	&& mkdir -p "$SYSROOT/share/cmake" \
	&& cp -r /usr/share/cmake/nlohmann_json "$SYSROOT/share/cmake/"

# El paquete de GLEW trae en su configuración de CMake la librería estática (GLEW::glew_s), pero no la instala, y CMake
# se niega a seguir. Se quita esa comprobación (awui usa la DLL)
RUN sed -i '/_cmake_import_check_.*GLEW::glew_s/d' \
		/usr/x86_64-w64-mingw32/sys-root/mingw/lib/cmake/glew/glew-targets-noconfig.cmake
