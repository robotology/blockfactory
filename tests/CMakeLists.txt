# Copyright (C) 2019 Istituto Italiano di Tecnologia (IIT). All rights reserved.
# This software may be modified and distributed under the terms of the
# GNU Lesser General Public License v2.1 or any later version.

add_library(Catch2Main main.cpp)
target_link_libraries(Catch2Main Catch2::Catch2)

add_blockfactory_test(Core "Core/SignalUnitTest.cpp")
