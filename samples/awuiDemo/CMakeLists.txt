cmake_minimum_required(VERSION 3.10)

project(awuiDemo CXX)

# Configuración estándar del proyecto
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS FALSE)

file(GLOB AWUIDEMO_SOURCES *.cpp)

if(WIN32)
	add_executable(awuiDemo WIN32)
else()
	add_executable(awuiDemo)
endif()

target_sources(awuiDemo PRIVATE
	${AWUIDEMO_SOURCES}
)

target_link_libraries(awuiDemo PRIVATE awui)

if(WIN32)
	add_custom_command(TARGET awuiDemo
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			$<TARGET_FILE:awui>
			$<TARGET_FILE_DIR:awuiDemo>)
endif()
