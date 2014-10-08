#!/bin/sh
rm CMakeCache.txt
cmake -G "CodeBlocks - Unix Makefiles" -DCMAKE_BUILD_TYPE=Release .
