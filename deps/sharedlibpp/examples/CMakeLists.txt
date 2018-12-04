# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

add_library(mymath MODULE MyMath.cpp MyMath.h)
target_include_directories(mymath PRIVATE $<TARGET_PROPERTY:shlibpp::shlibpp,INTERFACE_INCLUDE_DIRECTORIES>)
set_property(TARGET mymath PROPERTY PREFIX "")

add_executable(math_test math_test.cpp)
target_link_libraries(math_test PRIVATE shlibpp::shlibpp)

add_library(mymathcustom MODULE MyMathCustom.cpp MyMathCustom.h)
target_include_directories(mymathcustom PRIVATE $<TARGET_PROPERTY:shlibpp::shlibpp,INTERFACE_INCLUDE_DIRECTORIES>)
set_property(TARGET mymathcustom PROPERTY PREFIX "")

add_executable(math_test_custom math_test_custom.cpp)
target_link_libraries(math_test_custom PRIVATE shlibpp::shlibpp)
