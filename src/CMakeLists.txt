CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )

PROJECT( libawui )

#INCLUDE_DIRECTORIES( ../../libaw/debug/x11/include/ )
INCLUDE_DIRECTORIES( ../../libaw/debug/nt/include/ )

FILE(GLOB AWUI_FILES awui/*.cpp)

ADD_LIBRARY(awui STATIC ${AWUI_FILES})

TARGET_LINK_LIBRARIES(
	awui
)