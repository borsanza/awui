#!/bin/sh

# shellcheck disable=SC1090
MY_DIR="$(dirname "$0")"
. "${MY_DIR}/config/globals.sh"

if [ "$BUILD_TYPE" = "Debug" ]; then
	cd b/d/libawui || exit
else
	cd b/r/libawui || exit
fi

($MAKE_CLEAN)
cd ../../.. || exit

cd samples || exit
./clean.sh

