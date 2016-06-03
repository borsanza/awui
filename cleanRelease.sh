#!/bin/sh

my_dir="$(dirname "$0")"
. "$my_dir/config/globals.sh"

make clean
rm CMakeCache.txt
cmake -G "$EDITOR" -DCMAKE_BUILD_TYPE=Release .
make -j$CORES
cd samples
./cleanRelease.sh
