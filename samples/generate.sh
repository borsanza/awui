#!/bin/sh

# shellcheck disable=SC1090
MY_DIR="$(dirname "$0")"
. "$MY_DIR/../config/globals.sh"

rm CMakeCache.txt
cmake -G "$EDITOR" -DCMAKE_BUILD_TYPE="$BUILD_TYPE" -DCMAKE_ECLIPSE_VERSION="$ECLIPSE_VERSION" .
($COMMAND_BUILD)

