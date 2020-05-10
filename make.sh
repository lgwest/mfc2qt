#!/usr/bin/bash

mkdir -p build
pushd build >/dev/null
rm -f CMakeCache.txt
cmake -G "Visual Studio 14 2015" ..
cmake --build .
popd >/dev/null

