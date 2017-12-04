#!/bin/sh

my_dir="$(dirname "$0")"
. "$my_dir/../config/globals.sh"

ninja -t clean
rm CMakeCache.txt
cmake -G "$EDITOR" -DCMAKE_BUILD_TYPE=Debug .
ninja
