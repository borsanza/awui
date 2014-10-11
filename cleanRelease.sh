#!/bin/sh
make clean
rm CMakeCache.txt
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release .
make
cd samples
./cleanRelease.sh
