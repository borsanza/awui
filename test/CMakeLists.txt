CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )

PROJECT( awuiDemo )

INCLUDE_DIRECTORIES(
	../src/
#	../../libaw/debug/x11/include/
	../../libaw/debug/nt/include/
)

FILE(GLOB AWUIDEMO_SOURCES *.cpp)
FILE(GLOB AWUIDEMO_HEADERS *.h)

SOURCE_GROUP("Source" FILES ${AWUIDEMO_SOURCES})
SOURCE_GROUP("Headers" FILES ${AWUIDEMO_HEADERS})

ADD_EXECUTABLE(awuiDemo	${AWUIDEMO_SOURCES}	${AWUIDEMO_HEADERS})

TARGET_LINK_LIBRARIES(
	awuiDemo
	${OPENGL_LIBRARIES}
#	../../libaw/release/x11/libaw.a
	../../libaw/debug/nt/aw
	awui
)