#!/bin/bash
set -euvo pipefail


cd "$(git rev-parse --show-toplevel)"
test -e "rubble.gemspec"

git submodule update --init

cd contrib/SimpleBLE
rm -Rf build_*

mkdir build_simpleble
cd build_simpleble

# TODO: turn on address sanitizing/etc once things are working more
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS_DEBUG='-ggdb3 -O0' -DCMAKE_VERBOSE_MAKEFILE=ON -DBUILD_SHARED_LIBS=ON ../simpleble
cmake --build . -j7
cmake --install . --prefix install
sudo make install
