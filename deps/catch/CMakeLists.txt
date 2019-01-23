# Copyright (C) 2019 Istituto Italiano di Tecnologia (IIT). All rights reserved.
# This software may be modified and distributed under the terms of the
# GNU Lesser General Public License v2.1 or any later version.

add_library(Catch2 INTERFACE)
add_library(Catch2::Catch2 ALIAS Catch2)

target_include_directories(Catch2 INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
