cmake_minimum_required(VERSION 3.10)

project(stationTV CXX)

# Configuración estándar del proyecto
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS FALSE)

file(GLOB STATIONTV_SOURCES *.cpp)

add_executable(stationTV ${STATIONTV_SOURCES})

#if(WIN32 AND CMAKE_BUILD_TYPE STREQUAL "Release")
#    set_target_properties(stationTV PROPERTIES WIN32_EXECUTABLE TRUE)
#endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(MSVC)
        target_compile_options(stationTV PRIVATE /Od /Zi)
        target_link_options(stationTV PRIVATE /DEBUG)
    else()
        target_compile_options(stationTV PRIVATE -O0 -g)
    endif()
endif()

target_link_libraries(stationTV PRIVATE awui)

if(WIN32)
	add_custom_command(TARGET stationTV POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			$<TARGET_FILE:awui>
			$<TARGET_FILE_DIR:stationTV>)

	add_custom_command(TARGET stationTV POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
			${DLLS_PATH}
			$<TARGET_FILE_DIR:stationTV>)

	add_custom_command(TARGET stationTV POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			${CMAKE_CURRENT_SOURCE_DIR}/menu-settings.json
			$<TARGET_FILE_DIR:stationTV>)
endif()
