# Copia junto a un ejecutable de Windows las DLL que necesita (y las que necesitan esas, recursivamente), sacadas del
# propio ejecutable. Las de Windows (KERNEL32, opengl32...) no están en los directorios de búsqueda y se dejan.
#
#   cmake -DEXE=<exe> -DDIRS=<dir1|dir2...> -DOBJDUMP=<objdump> -P copy-runtime-dlls.cmake

# Desde Linux, CMake no sabe leer un .exe por su cuenta: se le dice que es PE y que use el objdump de MinGW
if(NOT CMAKE_HOST_WIN32)
	set(CMAKE_GET_RUNTIME_DEPENDENCIES_PLATFORM "windows+pe")
	set(CMAKE_GET_RUNTIME_DEPENDENCIES_TOOL "objdump")
	set(CMAKE_GET_RUNTIME_DEPENDENCIES_COMMAND "${OBJDUMP}")
endif()

string(REPLACE "|" ";" DIRS "${DIRS}")
get_filename_component(dest "${EXE}" DIRECTORY)

# CMake pasa a minúsculas los nombres de las DLL (en Windows da igual) y en Linux no encuentra "sdl2.dll" si el fichero
# es "SDL2.dll". Así que se buscan aquí, sin distinguir mayúsculas: nombre en minúsculas -> ruta
foreach(dir IN LISTS DIRS)
	file(GLOB dlls "${dir}/*.dll")
	foreach(dll IN LISTS dlls)
		get_filename_component(name "${dll}" NAME)
		string(TOLOWER "${name}" name)
		if(NOT DEFINED "path_${name}")
			set("path_${name}" "${dll}")
		endif()
	endforeach()
endforeach()

set(copied "")
set(pending "")

function(copy_dll path)
	get_filename_component(name "${path}" NAME)
	string(TOLOWER "${name}" name)
	if(name IN_LIST copied)
		return()
	endif()

	# Si la ha encontrado en la carpeta del ejecutable (una copia de antes), se copia la de los directorios de búsqueda
	if(DEFINED "path_${name}")
		set(path "${path_${name}}")
	endif()

	file(COPY "${path}" DESTINATION "${dest}")
	list(APPEND copied "${name}")
	list(APPEND pending "${path}")

	# sdl2-compat (el SDL2 de Fedora) carga SDL3.dll al arrancar, no la importa: no sale entre las dependencias
	if((name STREQUAL "sdl2.dll") AND DEFINED path_sdl3.dll)
		copy_dll("${path_sdl3.dll}")
	endif()

	set(copied "${copied}" PARENT_SCOPE)
	set(pending "${pending}" PARENT_SCOPE)
endfunction()

# Primero el ejecutable; luego, en cada vuelta, las DLL que se han encontrado a mano, para sacar las suyas
set(args EXECUTABLES "${EXE}")
while(TRUE)
	file(GET_RUNTIME_DEPENDENCIES
		${args}
		DIRECTORIES ${DIRS}
		RESOLVED_DEPENDENCIES_VAR resolved
		UNRESOLVED_DEPENDENCIES_VAR unresolved
		CONFLICTING_DEPENDENCIES_PREFIX conflict
		PRE_EXCLUDE_REGEXES "^api-ms-" "^ext-ms-"
		POST_EXCLUDE_REGEXES "[Ss]ystem32/"
	)

	# Al volver a compilar, la carpeta del ejecutable ya tiene una copia de cada DLL y puede salir dos veces: vale
	# cualquiera, copy_dll usa siempre la de los directorios de búsqueda
	foreach(name IN LISTS conflict_FILENAMES)
		list(GET "conflict_${name}" 0 path)
		list(APPEND resolved "${path}")
	endforeach()

	set(pending "")
	foreach(dll IN LISTS resolved)
		copy_dll("${dll}")
	endforeach()
	foreach(name IN LISTS unresolved)
		string(TOLOWER "${name}" name)
		if(DEFINED "path_${name}")
			copy_dll("${path_${name}}")
		endif()
	endforeach()

	if(NOT pending)
		break()
	endif()
	set(args LIBRARIES ${pending})
endwhile()

# fontconfig (lo usa pango para las fuentes) busca su configuración en etc/fonts junto a su DLL. Se copia la de MinGW
# (con su carpeta de fuentes de Windows y sus alias); los ficheros de conf.d son enlaces, se copia su contenido
if("libfontconfig-1.dll" IN_LIST copied)
	foreach(dir IN LISTS DIRS)
		get_filename_component(etc "${dir}/../etc/fonts" ABSOLUTE)
		if(EXISTS "${etc}/fonts.conf")
			file(GLOB_RECURSE configs RELATIVE "${etc}" "${etc}/*")
			foreach(config IN LISTS configs)
				configure_file("${etc}/${config}" "${dest}/etc/fonts/${config}" COPYONLY)
			endforeach()
			break()
		endif()
	endforeach()
endif()
