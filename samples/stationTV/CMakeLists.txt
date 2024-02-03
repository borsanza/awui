cmake_minimum_required(VERSION 3.10)

project(stationTV CXX)

# Configuración estándar del proyecto
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS FALSE)

file(GLOB STATIONTV_SOURCES *.cpp)

if(WIN32 AND CMAKE_BUILD_TYPE STREQUAL "Release")
	add_executable(stationTV WIN32)
else()
	add_executable(stationTV)
endif()

target_sources(stationTV PRIVATE
	${STATIONTV_SOURCES}
)

target_link_libraries(stationTV PRIVATE awui)

if(WIN32)
	add_custom_command(TARGET stationTV
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			$<TARGET_FILE:awui>
			$<TARGET_FILE_DIR:stationTV>)

	add_custom_command(TARGET stationTV
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
			${DLLS_PATH}
			$<TARGET_FILE_DIR:stationTV>)
endif()
