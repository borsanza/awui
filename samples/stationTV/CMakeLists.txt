awui_add_sample(stationTV FILES menu-settings.json stationtv.png lang fonts)

include(GNUInstallDirs)

# Dónde quedan los recursos al instalarlo: main.cpp los busca ahí si no están en la carpeta actual ni junto al
# ejecutable
set(STATIONTV_DATA_DIR "${CMAKE_INSTALL_FULL_DATADIR}/stationtv")
target_compile_definitions(stationTV PRIVATE STATIONTV_DATA_DIR="${STATIONTV_DATA_DIR}")

# Instalación (el paquete .deb: preset "package"):
#   lib/stationtv/      el ejecutable y libawui, que solo usa él (por eso no va en lib/)
#   bin/stationtv       enlace al ejecutable
#   share/stationtv/    imágenes, textos, fuentes, menú de ajustes y los juegos de CHIP-8 del Community Archive, que
#                       son CC0 (las demás ROMs no se pueden distribuir: cada uno pone las suyas en
#                       ~/.local/share/stationtv/roms)
if(UNIX AND NOT APPLE)
	set(private_lib_dir "lib/stationtv")
	set_target_properties(stationTV PROPERTIES INSTALL_RPATH "$ORIGIN")
	install(TARGETS stationTV awui
		RUNTIME DESTINATION ${private_lib_dir}
		LIBRARY DESTINATION ${private_lib_dir})

	file(CREATE_LINK "../${private_lib_dir}/stationTV" "${CMAKE_CURRENT_BINARY_DIR}/stationtv" SYMBOLIC)
	install(FILES "${CMAKE_CURRENT_BINARY_DIR}/stationtv" DESTINATION ${CMAKE_INSTALL_BINDIR})

	# Las imágenes y las ROMs están versionadas en build/samples/stationTV
	set(assets "${CMAKE_SOURCE_DIR}/build/samples/stationTV")
	install(FILES menu-settings.json stationtv.png DESTINATION ${CMAKE_INSTALL_DATADIR}/stationtv)
	install(DIRECTORY lang fonts "${assets}/images" DESTINATION ${CMAKE_INSTALL_DATADIR}/stationtv)
	install(DIRECTORY "${assets}/roms/chip8" DESTINATION ${CMAKE_INSTALL_DATADIR}/stationtv/roms)

	install(FILES package/stationtv.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
	install(FILES package/stationtv.svg DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps)
	install(FILES package/copyright DESTINATION ${CMAKE_INSTALL_DATADIR}/doc/stationtv)
endif()

# Windows: el icono va dentro del ejecutable, y la instalación (el instalador NSIS: scripts/build-windows.sh
# --installer) es una sola carpeta con el .exe, sus DLL y los recursos, como queda al compilar
if(WIN32)
	target_sources(stationTV PRIVATE package/stationtv.rc)

	install(TARGETS stationTV RUNTIME DESTINATION .)
	# Las DLL y la configuración de fuentes las deja junto al ejecutable la compilación (cmake/copy-runtime-dlls.cmake)
	install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/" DESTINATION .
		FILES_MATCHING PATTERN "*.dll"
		PATTERN "CMakeFiles" EXCLUDE PATTERN "lang" EXCLUDE PATTERN "fonts" EXCLUDE PATTERN "etc" EXCLUDE)
	install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/etc" DESTINATION .)

	set(assets "${CMAKE_SOURCE_DIR}/build/samples/stationTV")
	install(FILES menu-settings.json stationtv.png DESTINATION .)
	install(DIRECTORY lang fonts "${assets}/images" DESTINATION .)
	install(DIRECTORY "${assets}/roms/chip8" DESTINATION roms)
	install(FILES package/copyright DESTINATION . RENAME copyright.txt)
endif()
