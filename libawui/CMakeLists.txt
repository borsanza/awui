CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )

PROJECT( libawui )

if(UNIX)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=gnu++0x")
endif()

INCLUDE_DIRECTORIES(
	../libaw/
	/usr/include/cairo/
	.
#	"C:\\awui-project\\cairo\\include\\cairo\\"
)

FILE(GLOB_RECURSE LIBAWUI_SOURCES awui/*.cpp )
FILE(GLOB_RECURSE LIBAWUI_HEADERS awui/*.h )

SOURCE_GROUP("Source" FILES ${LIBAWUI_SOURCES})
SOURCE_GROUP("Headers" FILES ${LIBAWUI_HEADERS})

ADD_LIBRARY(awui STATIC ${LIBAWUI_SOURCES} ${LIBAWUI_HEADERS})

TARGET_LINK_LIBRARIES(
	awui
	aw
	cairo
# Valores para linux
	X11
	GL
#	"C:\\awui-project\\cairo\\lib\\libcairo.dll.a"
)
