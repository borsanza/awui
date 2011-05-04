CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )

PROJECT( libawui )

INCLUDE_DIRECTORIES(
	../libaw/
	/usr/include/cairomm-1.0/
	/usr/include/sigc++-2.0/
	/usr/lib/sigc++-2.0/include/
	/usr/include/cairo/
	/usr/include/freetype2/
)

FILE(GLOB LIBAWUI_SOURCES awui/*.cpp)
FILE(GLOB LIBAWUI_HEADERS awui/*.h)

SOURCE_GROUP("Source" FILES ${LIBAWUI_SOURCES})
SOURCE_GROUP("Headers" FILES ${LIBAWUI_HEADERS})

ADD_LIBRARY(awui STATIC ${LIBAWUI_SOURCES} ${LIBAWUI_HEADERS})

TARGET_LINK_LIBRARIES(
	awui
	aw
	/usr/lib/libcairomm-1.0.so
)
