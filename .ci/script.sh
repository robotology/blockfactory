#!/bin/sh
set -e
set -u

if [ "$TRAVIS_OS_NAME" = "windows" ] ; then
    INSTALL_PREFIX="$HOME\local"
    export CMAKE_PREFIX_PATH="${CMAKE_PREFIX_PATH:-};$INSTALL_PREFIX"
else
    INSTALL_PREFIX="$HOME/local"
    export CMAKE_PREFIX_PATH="${CMAKE_PREFIX_PATH:-}:$INSTALL_PREFIX"
fi

cd $TRAVIS_BUILD_DIR
mkdir -p build && cd build

if [ "$TRAVIS_CMAKE_GENERATOR" = "Visual Studio 15 2017" ] ; then
    # Build and install BlockFactory
    cmake -G"$TRAVIS_CMAKE_GENERATOR" \
          -A"${TRAVIS_CMAKE_ARCHITECTURE}" \
          -DCMAKE_INSTALL_PREFIX="$INSTALL_PREFIX" \
          -DBUILD_TESTING:BOOL=ON .. \
          ..
    cmake --build . --config $TRAVIS_BUILD_TYPE
    cmake --build . --target INSTALL
    # Build the example
    cd $TRAVIS_BUILD_DIR/example
    mkdir build && cd build
    cmake -G"$TRAVIS_CMAKE_GENERATOR" -A"${TRAVIS_CMAKE_ARCHITECTURE}" ..
    cmake --build . --config $TRAVIS_BUILD_TYPE
else
    # Build and install BlockFactory
    cmake -G"$TRAVIS_CMAKE_GENERATOR" \
          -DCMAKE_BUILD_TYPE=$TRAVIS_BUILD_TYPE \
          -DCMAKE_INSTALL_PREFIX="$INSTALL_PREFIX" \
          -DBUILD_TESTING:BOOL=ON .. \
          ..
    cmake --build .
    cmake --build . --target install
    # Build the example
    cd $TRAVIS_BUILD_DIR/example
    mkdir build && cd build
    cmake -G"$TRAVIS_CMAKE_GENERATOR" -DCMAKE_BUILD_TYPE=$TRAVIS_BUILD_TYPE ..
    cmake --build .
fi

# Run the unit tests with ctest
cd $TRAVIS_BUILD_DIR/build
ctest --output-on-failure

if [ "${VALGRIND_TESTS:-0}" -eq 1 ] ; then
    cd $TRAVIS_BUILD_DIR/build
    ctest -T memcheck --output-on-failure  || FAILED=1

    if [ ${FAILED:-0} -eq 1 ] ; then
        for log in $(ls Testing/Temporary/MemoryChecker.*.log) ; do
            echo
            echo "$log"
            echo
            cat $log
        done
        exit 1
    fi

fi
