#!/usr/bin/bash

mkdir -p build
pushd build 2>/dev/null
rm -f CMakeCache.txt
cmake ..
cmake --build .
popd 2>/dev/null

