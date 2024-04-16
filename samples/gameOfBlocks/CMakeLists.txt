cmake_minimum_required(VERSION 3.10)

project(gameOfBlocks CXX)

# Configuración estándar del proyecto
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS FALSE)

file(GLOB GAMEOFBLOCKS_SOURCES *.cpp)

add_executable(gameOfBlocks ${GAMEOFBLOCKS_SOURCES})

#if(WIN32 AND CMAKE_BUILD_TYPE STREQUAL "Release")
#    set_target_properties(gameOfBlocks PROPERTIES WIN32_EXECUTABLE TRUE)
#endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(MSVC)
        target_compile_options(gameOfBlocks PRIVATE /Od /Zi)
        target_link_options(gameOfBlocks PRIVATE /DEBUG)
    else()
        target_compile_options(gameOfBlocks PRIVATE -O0 -g)
    endif()
endif()

target_link_libraries(gameOfBlocks PRIVATE awui)

if(WIN32)
	add_custom_command(TARGET gameOfBlocks POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			$<TARGET_FILE:awui>
			$<TARGET_FILE_DIR:gameOfBlocks>)

	add_custom_command(TARGET gameOfBlocks POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
			${DLLS_PATH}
			$<TARGET_FILE_DIR:gameOfBlocks>)
endif()
