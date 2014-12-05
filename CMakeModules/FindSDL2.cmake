IF(SDL2_INCLUDE_DIR AND SDL2_LIBRARY)
    SET(SDL2_FOUND TRUE)
ELSE(SDL2_INCLUDE_DIR AND SDL2_LIBRARY)
    # Linux
    FIND_PATH(
        SDL2_INCLUDE_DIR
        SDL2/SDL.h
    )

    FIND_LIBRARY(SDL2_LIBRARY
        SDL2
    )

    # Windows
    FIND_PATH(
        SDL2_INCLUDE_DIR
        SDL2/SDL.h
        PATHS
        "C:/Program Files/SDL"
        NO_DEFAULT_PATH
    )

    FIND_LIBRARY(SDL2_LIBRARY
        sdl2
        "C:/Program Files/SDL/lib/x86"
        NO_DEFAULT_PATH
    )

    # Mac
    FIND_PATH(
        SDL2_INCLUDE_DIR
        SDL2/SDL.h
        PATHS
        /opt/local/include/
        NO_DEFAULT_PATH
    )

    FIND_LIBRARY(SDL2_LIBRARY
        sdl2
        /opt/local/lib
        NO_DEFAULT_PATH
    )

    INCLUDE(FindPackageHandleStandardArgs)
    FIND_PACKAGE_HANDLE_STANDARD_ARGS(sdl2 DEFAULT_MSG SDL2_INCLUDE_DIR SDL2_LIBRARY)
    MARK_AS_ADVANCED(SDL2_INCLUDE_DIR SDL2_LIBRARY)
ENDIF(SDL2_INCLUDE_DIR AND SDL2_LIBRARY)

