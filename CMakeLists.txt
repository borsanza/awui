cmake_minimum_required(VERSION 3.16)

# No permitir compilar dentro del código fuente: los samples necesitan los assets de build/samples/
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
	message(FATAL_ERROR "No se puede compilar en el directorio del código fuente.\n"
		"Usa:\n"
		"  cmake --preset release && cmake --build --preset release\n"
		"o bien:\n"
		"  cmake -S . -B build && cmake --build build\n"
		"Borra antes CMakeCache.txt y CMakeFiles/ que se acaban de crear aquí.")
endif()

project(AwuiProject LANGUAGES C CXX)

# Release por defecto si no se indica tipo de compilación (generadores de una sola configuración)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	set(CMAKE_BUILD_TYPE Release CACHE STRING "Tipo de compilación" FORCE)
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release RelWithDebInfo MinSizeRel)
endif()

# Configuración común a todos los targets
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS FALSE)

# Genera compile_commands.json para clangd / IntelliSense
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# En Linux usa libOpenGL (GLVND) en vez de la libGL legacy
set(OpenGL_GL_PREFERENCE GLVND)

option(AWUI_WARNINGS "Activa los warnings del compilador" OFF)

if(AWUI_WARNINGS)
	if(MSVC)
		add_compile_options(/W4)
	else()
		add_compile_options(-Wall -Wextra)
	endif()
endif()

# AddressSanitizer + UndefinedBehaviorSanitizer para desarrollo: detectan accesos fuera de memoria, usos tras
# liberar, delete dobles, desbordamientos de enteros con signo, etc. en el momento en que ocurren.
# Va unas 2-3 veces más lento; usar con RelWithDebInfo o Debug (preset "sanitize")
option(AWUI_SANITIZE "Compila con AddressSanitizer y UndefinedBehaviorSanitizer" OFF)

if(AWUI_SANITIZE)
	if(MSVC)
		# MSVC solo tiene AddressSanitizer
		add_compile_options(/fsanitize=address)
	else()
		add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
		add_link_options(-fsanitize=address,undefined)
	endif()
endif()

add_subdirectory(third_party)
add_subdirectory(libawui)
add_subdirectory(samples)

# Paquetes de StationTV, con CPack. Lo que se instala está en samples/stationTV/CMakeLists.txt
#   Ubuntu: paquete .deb (scripts/build-deb.sh, preset "package")
#   Windows: instalador NSIS (scripts/build-windows.sh --installer, preset "windows")
set(CPACK_PACKAGE_NAME "stationtv")
set(CPACK_PACKAGE_VERSION "0.1.0")
set(CPACK_PACKAGE_VENDOR "Borja Sánchez Zamorano")
set(CPACK_PACKAGE_CONTACT "Borja Sánchez Zamorano <borsanza@gmail.com>")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Menú para la tele con emuladores retro")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/borsanza/awui")

if(WIN32)
	set(CPACK_GENERATOR "NSIS")
	set(CPACK_PACKAGE_FILE_NAME "StationTV-${CPACK_PACKAGE_VERSION}-instalador")
	set(CPACK_PACKAGE_INSTALL_DIRECTORY "StationTV")
	set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "StationTV")
	set(CPACK_NSIS_PACKAGE_NAME "StationTV")
	set(CPACK_NSIS_DISPLAY_NAME "StationTV")
	set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
	set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/samples/stationTV/package/stationtv.ico")
	set(CPACK_NSIS_MUI_UNIICON "${CMAKE_SOURCE_DIR}/samples/stationTV/package/stationtv.ico")
	set(CPACK_NSIS_INSTALLED_ICON_NAME "stationTV.exe")
	set(CPACK_NSIS_URL_INFO_ABOUT "${CPACK_PACKAGE_HOMEPAGE_URL}")
	set(CPACK_NSIS_CONTACT "${CPACK_PACKAGE_CONTACT}")
	# Al instalar encima de otra versión, primero se desinstala la anterior
	set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
	# Acceso directo en el menú Inicio y en el escritorio (el del escritorio, a mano: CPACK_CREATE_DESKTOP_LINKS solo
	# lo crea si se enseña la página de opciones, que también ofrece añadirlo al PATH)
	set(CPACK_PACKAGE_EXECUTABLES "stationTV" "StationTV")
	set(CPACK_NSIS_CREATE_ICONS_EXTRA "CreateShortCut '$DESKTOP\\\\StationTV.lnk' '$INSTDIR\\\\stationTV.exe'")
	set(CPACK_NSIS_DELETE_ICONS_EXTRA "Delete '$DESKTOP\\\\StationTV.lnk'")
	# Al terminar, ofrece abrirlo
	set(CPACK_NSIS_MUI_FINISHPAGE_RUN "stationTV.exe")
	include(CPack)
elseif(UNIX AND NOT APPLE)
	set(CPACK_PACKAGE_DESCRIPTION "Menú a pantalla completa, pensado para la tele y el mando, desde el que se juega a CHIP-8, Sega Master System, Game Gear, SG-1000 y ZX Spectrum. Trae los juegos de CHIP-8 del Chip-8 Community Archive (CC0); las demás ROMs van en ~/.local/share/stationtv/roms.")
	set(CPACK_GENERATOR "DEB")
	set(CPACK_PACKAGING_INSTALL_PREFIX "/usr")
	set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
	set(CPACK_DEBIAN_PACKAGE_SECTION "games")
	set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "${CPACK_PACKAGE_HOMEPAGE_URL}")
	# Las dependencias (SDL2, cairo, pango...) las calcula dpkg-shlibdeps; libawui va dentro del paquete
	set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
	set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS_PRIVATE_DIRS "${CMAKE_BINARY_DIR}/libawui")
	include(CPack)
endif()
