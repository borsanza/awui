# Código de terceros, sin modificar. Cada uno es una librería estática que enlaza quien la necesite

# emu2413: emulador del chip FM YM2413 (sonido FM de la Master System japonesa). Lo usa libawui
add_library(emu2413 STATIC emu2413/emu2413.c)
target_include_directories(emu2413 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/emu2413")
# Va dentro de una librería compartida (libawui)
set_target_properties(emu2413 PROPERTIES POSITION_INDEPENDENT_CODE ON)

# Desplaza negativos a la izquierda para duplicarlos: en C es indefinido, pero todos los compiladores lo hacen bien.
# Se quita esa comprobación para que UBSan solo avise de lo nuestro
if(AWUI_SANITIZE AND NOT MSVC)
	target_compile_options(emu2413 PRIVATE -fno-sanitize=shift)
endif()
