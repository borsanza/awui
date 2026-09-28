if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(DLLS_PATH "${CMAKE_SOURCE_DIR}/ext/lib64")
else()
	set(DLLS_PATH "${CMAKE_SOURCE_DIR}/ext/lib32")
endif()

# awui_add_sample(<nombre> [GUI_RELEASE] [FILES fichero...])
#   GUI_RELEASE: en Windows, compila en Release sin consola
#   FILES: ficheros del directorio del sample a copiar junto al ejecutable
function(awui_add_sample name)
	cmake_parse_arguments(ARG "GUI_RELEASE" "" "FILES" ${ARGN})

	file(GLOB sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
	add_executable(${name} ${sources})
	target_link_libraries(${name} PRIVATE awui)

	if(ARG_GUI_RELEASE)
		set_target_properties(${name} PROPERTIES WIN32_EXECUTABLE $<CONFIG:Release>)
	endif()

	foreach(file IN LISTS ARG_FILES)
		add_custom_command(TARGET ${name} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different
				"${CMAKE_CURRENT_SOURCE_DIR}/${file}"
				$<TARGET_FILE_DIR:${name}>)
	endforeach()

	# En Windows no hay rpath: la DLL de awui y las de ext/ tienen que estar junto al ejecutable
	if(WIN32)
		add_custom_command(TARGET ${name} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different
				$<TARGET_FILE:awui>
				$<TARGET_FILE_DIR:${name}>)

		if(EXISTS "${DLLS_PATH}")
			add_custom_command(TARGET ${name} POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E copy_directory
					"${DLLS_PATH}"
					$<TARGET_FILE_DIR:${name}>)
		endif()
	endif()
endfunction()

add_subdirectory(stationTV)
add_subdirectory(awuiDemo)
add_subdirectory(awSlider)
add_subdirectory(awTest)
add_subdirectory(gameOfBlocks)
