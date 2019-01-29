/*
 * Copyright (C) 2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * GNU Lesser General Public License v2.1 or any later version.
 */

#include "BlockFactory/Core/Block.h"
#include "BlockFactory/Core/FactorySingleton.h"

#include <catch2/catch.hpp>
#include <iostream>

using namespace blockfactory::core;

const std::string mockBlockName = "MockBlock";
const std::string mockPluginName = "MockPlugin";

TEST_CASE("Load plugin", "[Factory][Plugin]")
{
    auto& factorySingleton = blockfactory::core::ClassFactorySingleton::getInstance();

    {
        // Load unexistent plugin
        auto factory = factorySingleton.getClassFactory({"wrongPluginName", "wrongBlockName"});
        REQUIRE(factory == nullptr);
    }

    {
        // Load unexistent plugin
        auto factory = factorySingleton.getClassFactory({"wrongPluginName", mockBlockName});
        REQUIRE(factory == nullptr);
    }

    {
        // Load existent plugin and ask a non-existing class
        auto factory = factorySingleton.getClassFactory({mockPluginName, "wrongBlockName"});
        REQUIRE(factory == nullptr);
    }

    {
        // Load with right parameters, but the linker should not find the plugin library
        // since the folder is not in the search path
        //        auto factory = factorySingleton.getClassFactory({mockPluginName, mockBlockName});
        //        REQUIRE(factory == nullptr);
    }

    // Extend the plugin search path
    factorySingleton.extendPluginSearchPath(TEST_EXTENDED_PLUGIN_PATH);

    {
        // Now the plugin should get loaded, but we ask a non-existing class
        auto factory = factorySingleton.getClassFactory({mockPluginName, "wrongBlockName"});
        REQUIRE(factory == nullptr);
    }

    // Finally, this should work
    auto factory = factorySingleton.getClassFactory({mockPluginName, mockBlockName});
    REQUIRE(factory != nullptr);

    // Deallocate the factory
    factory.reset();
    REQUIRE(factorySingleton.destroyFactory({mockPluginName, mockBlockName}));
}

TEST_CASE("Load and use plugin", "[Factory][Plugin][Parameter]")
{
    // Get the factory singleton
    auto& factorySingleton = blockfactory::core::ClassFactorySingleton::getInstance();

    // Extend the plugin search path
    factorySingleton.extendPluginSearchPath(TEST_EXTENDED_PLUGIN_PATH);

    // Load the plugin and get the factory
    auto factory = factorySingleton.getClassFactory({mockPluginName, mockBlockName});
    REQUIRE(factory != nullptr);

    // Allocate a mock::MockBlock object and get its core::Block pointer
    blockfactory::core::Block* block = factory->create();
    factory->addRef();
    REQUIRE(block != nullptr);

    // Get the block parameters
    blockfactory::core::Parameters params;
    REQUIRE(block->getParameters(params) == true);

    // Get the store parameter
    int paramValue;
    const int expectedParamValue = 42;
    REQUIRE(params.existName("mockParam"));
    REQUIRE(params.getParameter("mockParam", paramValue));
    REQUIRE(paramValue == expectedParamValue);

    // Try to run output(). It should just return true.
    REQUIRE(block->output(nullptr) == true);

    // Deallocate the block
    factory->destroy(block);
    factory->removeRef();

    // Deallocate the factory
    factory.reset();
    REQUIRE(factorySingleton.destroyFactory({mockPluginName, mockBlockName}));
}
