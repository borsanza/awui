cmake_minimum_required(VERSION 3.10)

project(libawui)

# Configuración estándar del proyecto
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS FALSE)

find_package(PkgConfig REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(nlohmann_json REQUIRED)
pkg_search_module(SDL2 REQUIRED sdl2)
pkg_search_module(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)
pkg_search_module(CAIRO REQUIRED cairo)

# Define los archivos de fuente y cabecera
file(GLOB_RECURSE LIBAWUI_SOURCES awui/*.cpp )
file(GLOB_RECURSE LIBAWUI_HEADERS awui/*.h )

# Organiza los archivos en grupos visuales para IDEs
source_group("Source" FILES ${LIBAWUI_SOURCES})
source_group("Headers" FILES ${LIBAWUI_HEADERS})

# Define el target de la biblioteca
add_library(awui SHARED ${LIBAWUI_SOURCES} ${LIBAWUI_HEADERS})

# Especifica directorios de inclusión para el target
target_include_directories(awui PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CAIRO_INCLUDE_DIRS}
  ${GLEW_INCLUDE_DIRS}
  ${OPENGL_INCLUDE_DIR}
  ${SDL2_INCLUDE_DIRS}
  ${SDL2IMAGE_INCLUDE_DIRS}
)

# Vincula el target con las bibliotecas necesarias
target_link_libraries(awui PRIVATE
  ${CAIRO_LIBRARIES}
  ${GLEW_LIBRARIES}
  ${OPENGL_gl_LIBRARY}
  ${SDL2_LIBRARIES}
  ${SDL2IMAGE_LIBRARIES}
  nlohmann_json::nlohmann_json
)

# Agrega definiciones de compilación para el target
target_compile_definitions(awui PRIVATE ${CAIRO_CFLAGS_OTHER})
