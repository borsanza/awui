CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )

PROJECT( libawui )

if(UNIX)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=gnu++0x")
endif()

INCLUDE_DIRECTORIES(
	/usr/include/cairo/
	.
	${SDL_INCLUDE_DIR}
#	"C:\\awui-project\\cairo\\include\\cairo\\"
)


FILE(GLOB_RECURSE LIBAWUI_SOURCES awui/*.cpp )
FILE(GLOB_RECURSE LIBAWUI_HEADERS awui/*.h )

SOURCE_GROUP("Source" FILES ${LIBAWUI_SOURCES})
SOURCE_GROUP("Headers" FILES ${LIBAWUI_HEADERS})

ADD_LIBRARY(awui STATIC ${LIBAWUI_SOURCES} ${LIBAWUI_HEADERS})

TARGET_LINK_LIBRARIES(
	awui
	cairo
	${SDL_LIBRARY}
	${SDLIMAGE_LIBRARY}
# Valores para linux
	GL
#	"C:\\awui-project\\cairo\\lib\\libcairo.dll.a"
)
