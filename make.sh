#!/usr/bin/bash

mkdir -p build
pushd build 2>/dev/null
cmake -G "Visual Studio 14 2015" ..
cmake --build .
popd 2>/dev/null

