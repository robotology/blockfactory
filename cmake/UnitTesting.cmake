# Copyright (C) 2019 Istituto Italiano di Tecnologia (IIT). All rights reserved.
# This software may be modified and distributed under the terms of the
# GNU Lesser General Public License v2.1 or any later version.

# Tests should be executed as follows:
#
# cd build
# ctest --output-on-failure
# ctest -T memcheck --output-on-failure

# Enable valgrind tests
# https://gitlab.kitware.com/cmake/community/wikis/doc/ctest/Testing-With-CTest#dynamic-analysis
# These variable should be defined before including CTest
find_program(MEMORYCHECK_COMMAND valgrind)
set(MEMORYCHECK_COMMAND_OPTIONS
    "-q --tool=memcheck --leak-check=yes --show-reachable=yes --error-exitcode=1 --num-callers=50")

include(CTest)
include(Catch)
enable_testing()

function(add_blockfactory_test name unit_test_files)
    set(targetname ${name}UnitTests)
    add_executable(${targetname}
        ${unit_test_files})

    target_link_libraries(${targetname} PRIVATE Catch2Main BlockFactory::Core)
    catch_discover_tests(${targetname} EXTRA_ARGS "--use-colour yes")
endfunction()
