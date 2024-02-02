cmake_minimum_required(VERSION 3.5)
project(Todo)

# Configura el directorio de salida para los binarios
IF (CMAKE_BUILD_TYPE STREQUAL "Release")
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/b/r)
ELSE()
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/b/d)
ENDIF()

# Incluye subproyectos
add_subdirectory(libawui)
add_subdirectory(samples)