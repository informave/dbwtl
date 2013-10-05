#!/bin/sh

mkdir build
mkdir _install
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=../_install $OPTIONS || exit $?
cmake --build . --target install || exit $?
ctest -V . || exit $?


