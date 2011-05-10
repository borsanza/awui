CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )

PROJECT( libawui )

INCLUDE_DIRECTORIES(
	../libaw/
	/usr/include/cairo/
#	"C:\\Archivos de programa\\Archivos comunes\\GTK\\2.0\\include\\cairo\\"
)

FILE(GLOB LIBAWUI_SOURCES awui/*.cpp)
FILE(GLOB LIBAWUI_HEADERS awui/*.h)

SOURCE_GROUP("Source" FILES ${LIBAWUI_SOURCES})
SOURCE_GROUP("Headers" FILES ${LIBAWUI_HEADERS})

ADD_LIBRARY(awui STATIC ${LIBAWUI_SOURCES} ${LIBAWUI_HEADERS})

TARGET_LINK_LIBRARIES(
	awui
	aw
	cairo
#	"C:\\Archivos de programa\\Archivos comunes\\GTK\\2.0\\lib\\libcairo.dll.a"
)
