#!/bin/sh
set -e
set -u

cd $TRAVIS_BUILD_DIR
mkdir -p build && cd build

if [ "$TRAVIS_CMAKE_GENERATOR" = "Visual Studio 15 2017" ] ; then
    cmake -G"$TRAVIS_CMAKE_GENERATOR" -A"${TRAVIS_CMAKE_ARCHITECTURE}" ..
    cmake --build . --config $TRAVIS_BUILD_TYPE
else
    cmake -G"$TRAVIS_CMAKE_GENERATOR" -DCMAKE_BUILD_TYPE=$TRAVIS_BUILD_TYPE ..
    cmake --build .
fi
