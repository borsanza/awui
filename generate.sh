#!/bin/sh

# shellcheck disable=SC1090
MY_DIR="$(dirname "$0")"
. "${MY_DIR}/config/globals.sh"

rm CMakeCache.txt
cmake -G "$EDITOR" -DCMAKE_BUILD_TYPE="$BUILD_TYPE" -DCMAKE_ECLIPSE_VERSION="$ECLIPSE_VERSION" -DCMAKE_ECLIPSE_GENERATE_SOURCE_PROJECT=TRUE .
($COMMAND_BUILD)
cd samples || exit
./generate.sh

