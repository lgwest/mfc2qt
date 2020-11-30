#!/usr/bin/bash

mkdir -p build
pushd build >/dev/null
rm -f CMakeCache.txt
cmake -G "Visual Studio 16 2019" ..
cmake --build .
popd >/dev/null

