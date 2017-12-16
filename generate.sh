#!/bin/sh

# shellcheck disable=SC1090
MY_DIR="$(dirname "$0")"
. "${MY_DIR}/config/globals.sh"

if [ "$BUILD_TYPE" = "Debug" ]; then
	mkdir -p b/d/libawui
	cd b/d/libawui || exit
else
	mkdir -p b/r/libawui
	cd b/r/libawui || exit
fi

rm CMakeCache.txt
cmake -G "$EDITOR" -DCMAKE_BUILD_TYPE="Debug" -DCMAKE_ECLIPSE_VERSION="$ECLIPSE_VERSION" ../../../libawui
($COMMAND_BUILD)
cd ../../.. || exit

cd samples || exit
./generate.sh

