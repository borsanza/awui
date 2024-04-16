cmake_minimum_required(VERSION 3.10)

project(AwuiSamples)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(DLLS_PATH "${CMAKE_SOURCE_DIR}/ext/lib64")
else()
    set(DLLS_PATH "${CMAKE_SOURCE_DIR}/ext/lib32")
endif()

add_subdirectory(stationTV)
add_subdirectory(awuiDemo)
add_subdirectory(awSlider)
add_subdirectory(awTest)
add_subdirectory(gameOfBlocks)
