#!/bin/sh
set -e
set -u

cd $TRAVIS_BUILD_DIR
mkdir -p build && cd build

if [ "$TRAVIS_CMAKE_GENERATOR" = "Visual Studio 15 2017" ] ; then
    # Build and install BlockFactory
    cmake -G"$TRAVIS_CMAKE_GENERATOR" -A"${TRAVIS_CMAKE_ARCHITECTURE}" ..
    cmake --build . --config $TRAVIS_BUILD_TYPE
    cmake --build . --target INSTALL
    # Build the example
    cd $TRAVIS_BUILD_DIR/example
    cmake -G"$TRAVIS_CMAKE_GENERATOR" -A"${TRAVIS_CMAKE_ARCHITECTURE}" ..
    cmake --build . --config $TRAVIS_BUILD_TYPE
else
    # Build and install BlockFactory
    cmake -G"$TRAVIS_CMAKE_GENERATOR" -DCMAKE_BUILD_TYPE=$TRAVIS_BUILD_TYPE ..
    cmake --build .
    cmake --build . --target install
    # Build the example
    cd $TRAVIS_BUILD_DIR/example
    cmake -G"$TRAVIS_CMAKE_GENERATOR" -DCMAKE_BUILD_TYPE=$TRAVIS_BUILD_TYPE ..
    cmake --build .
fi
