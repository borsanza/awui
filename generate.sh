#!/bin/sh

# shellcheck disable=SC1090
MY_DIR="$(dirname "$0")"
. "${MY_DIR}/config/globals.sh"

if [ "$BUILD_TYPE" = "Debug" ]; then
	mkdir -p b/libawui/d/
	cd b/libawui/d/ || exit
else
	mkdir -p b/libawui/r/
	cd b/libawui/r/ || exit
fi

rm CMakeCache.txt
cmake -G "$EDITOR" -DCMAKE_BUILD_TYPE="Debug" -DCMAKE_ECLIPSE_VERSION="$ECLIPSE_VERSION" -DCMAKE_ECLIPSE_GENERATE_SOURCE_PROJECT=TRUE ../../../libawui
($COMMAND_BUILD)
cd ../../.. || exit

cd samples || exit
./generate.sh

