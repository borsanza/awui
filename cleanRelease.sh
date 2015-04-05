#!/bin/sh
make clean
rm CMakeCache.txt
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release .
make -j8
cd samples
./cleanRelease.sh

