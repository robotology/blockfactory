# Copyright (C) 2019 Istituto Italiano di Tecnologia (IIT). All rights reserved.
# This software may be modified and distributed under the terms of the
# GNU Lesser General Public License v2.1 or any later version.

add_library(Catch2Main main.cpp)
target_link_libraries(Catch2Main Catch2::Catch2)

include(BlockFactoryPlugin)
register_blockfactory_block(
    BLOCK_NAME MockBlock
    PLUGIN_NAME MockPlugin
    SOURCES "Factory/MockPlugin.h"
            "Factory/MockPlugin.cpp")
add_blockfactory_plugin(MockPlugin)

add_blockfactory_test(
    NAME Core
    SOURCES "Core/SignalUnitTest.cpp")

add_blockfactory_test(
    NAME Factory
    SOURCES "Factory/FactoryUnitTest.cpp")
target_compile_definitions(FactoryUnitTests PRIVATE TEST_EXTENDED_PLUGIN_PATH="$<TARGET_FILE_DIR:MockPlugin>")
