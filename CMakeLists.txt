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
