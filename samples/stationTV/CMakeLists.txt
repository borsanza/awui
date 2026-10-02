awui_add_sample(stationTV FILES menu-settings.json lang fonts)

include(GNUInstallDirs)

# Dónde quedan los recursos al instalarlo: main.cpp los busca ahí si no están en la carpeta actual ni junto al
# ejecutable
set(STATIONTV_DATA_DIR "${CMAKE_INSTALL_FULL_DATADIR}/stationtv")
target_compile_definitions(stationTV PRIVATE STATIONTV_DATA_DIR="${STATIONTV_DATA_DIR}")

# Instalación (el paquete .deb: preset "package"):
#   lib/stationtv/      el ejecutable y libawui, que solo usa él (por eso no va en lib/)
#   bin/stationtv       enlace al ejecutable
#   share/stationtv/    imágenes, textos, fuentes, menú de ajustes y las ROMs de CHIP-8 (las demás no se pueden
#                       distribuir: cada uno pone las suyas en ~/.local/share/stationtv/roms)
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
	install(FILES menu-settings.json DESTINATION ${CMAKE_INSTALL_DATADIR}/stationtv)
	install(DIRECTORY lang fonts "${assets}/images" DESTINATION ${CMAKE_INSTALL_DATADIR}/stationtv)
	install(DIRECTORY "${assets}/roms/chip8" DESTINATION ${CMAKE_INSTALL_DATADIR}/stationtv/roms)

	install(FILES package/stationtv.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
	install(FILES package/stationtv.svg DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps)
	install(FILES package/copyright DESTINATION ${CMAKE_INSTALL_DATADIR}/doc/stationtv)
endif()
