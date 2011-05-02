CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )

PROJECT( awuiDemo )

INCLUDE_DIRECTORIES(
	../src/
#	../../libaw/debug/x11/include/
	../../libaw/debug/nt/include/
)

FILE(GLOB AWUIDEMO_FILES *.cpp)

ADD_EXECUTABLE(awuiDemo	${AWUIDEMO_FILES})

TARGET_LINK_LIBRARIES(
	awuiDemo
	${OPENGL_LIBRARIES}
#	/home/borsanza/opengl/libaw/release/x11/libaw.a
	../../libaw/debug/nt/aw
#	/home/borsanza/opengl/awui/src/libawui.so
	../src/Debug/awui
)