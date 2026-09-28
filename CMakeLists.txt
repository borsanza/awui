cmake_minimum_required(VERSION 3.16)

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

add_subdirectory(libawui)
add_subdirectory(samples)
