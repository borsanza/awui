#!/bin/sh

# shellcheck disable=SC2034

BUILD_TYPE="Debug"
USE_MAKE=1
#ECLIPSE_VERSION="4.7.1a"

if [ "$USE_MAKE" -eq "1" ]; then
	CORES=$(nproc)
	EDITOR="Unix Makefiles"
	COMMAND_BUILD="make -j${CORES}"
	MAKE_CLEAN="make clean"
else
	EDITOR="Eclipse CDT4 - Ninja"
	COMMAND_BUILD="ninja"
	MAKE_CLEAN="ninja -t clean"
fi
