#!/bin/sh

# shellcheck disable=SC1090
MY_DIR="$(dirname "$0")"
. "${MY_DIR}/config/globals.sh"

($MAKE_CLEAN)
cd samples || exit
./clean.sh

