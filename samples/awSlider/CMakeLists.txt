cmake_minimum_required(VERSION 3.10)

project(awSlider CXX)

# Configuración estándar del proyecto
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS FALSE)

file(GLOB AWSLIDER_SOURCES *.cpp)

if(WIN32)
	add_executable(awSlider WIN32)
else()
	add_executable(awSlider)
endif()

target_sources(awSlider PRIVATE
	${AWSLIDER_SOURCES}
)

target_link_libraries(awSlider PRIVATE awui)

if(WIN32)
	add_custom_command(TARGET awSlider
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			$<TARGET_FILE:awui>
			$<TARGET_FILE_DIR:awSlider>)

	add_custom_command(TARGET awSlider
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
			${DLLS_PATH}
			$<TARGET_FILE_DIR:awSlider>)
endif()
