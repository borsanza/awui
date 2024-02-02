cmake_minimum_required(VERSION 3.10)

project(awTest CXX)

# Configuración estándar del proyecto
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS FALSE)

file(GLOB AWTEST_SOURCES *.cpp)

if(WIN32)
	add_executable(awTest WIN32)
else()
	add_executable(awTest)
endif()

target_sources(awTest PRIVATE
	${AWTEST_SOURCES}
)

target_link_libraries(awTest PRIVATE awui)

if(WIN32)
	add_custom_command(TARGET awTest
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			$<TARGET_FILE:awui>
			$<TARGET_FILE_DIR:awTest>)
endif()
