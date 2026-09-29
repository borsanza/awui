find_package(PkgConfig REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(nlohmann_json REQUIRED)
pkg_search_module(SDL2 REQUIRED sdl2)
pkg_search_module(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)
pkg_search_module(CAIRO REQUIRED cairo)

# Define los archivos de fuente y cabecera (CONFIGURE_DEPENDS: detecta ficheros nuevos sin reconfigurar a mano)
file(GLOB_RECURSE LIBAWUI_SOURCES CONFIGURE_DEPENDS awui/*.cpp awui/*.c)
file(GLOB_RECURSE LIBAWUI_HEADERS CONFIGURE_DEPENDS awui/*.h)

# Organiza los archivos en grupos visuales para IDEs
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/awui" FILES ${LIBAWUI_SOURCES} ${LIBAWUI_HEADERS})

add_library(awui SHARED ${LIBAWUI_SOURCES} ${LIBAWUI_HEADERS})

# Con MSVC exporta todos los símbolos de la DLL (MinGW ya lo hace por defecto)
set_target_properties(awui PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)

# Las cabeceras de awui incluyen SDL, OpenGL, cairo y json, así que las dependencias son públicas
target_include_directories(awui PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}"
	${CAIRO_INCLUDE_DIRS}
	${SDL2_INCLUDE_DIRS}
	${SDL2IMAGE_INCLUDE_DIRS}
)

# *_LINK_LIBRARIES contiene rutas completas, así funciona también con prefijos no estándar (MSYS2, Homebrew...)
target_link_libraries(awui PUBLIC
	${CAIRO_LINK_LIBRARIES}
	${SDL2_LINK_LIBRARIES}
	${SDL2IMAGE_LINK_LIBRARIES}
	GLEW::GLEW
	OpenGL::GL
	nlohmann_json::nlohmann_json
)

target_compile_options(awui PUBLIC ${CAIRO_CFLAGS_OTHER})
