/*
 * Copyright (C) Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * GNU Lesser General Public License v2.1 or any later version.
 */

#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "BlockFactory/Core/Block.h"
#include "BlockFactory/Core/FactorySingleton.h"

int main(int argc, char* argv[])
{
    std::string commandName = "blockfactory-exists";
    if (argc != 3)
    {
        std::cout << commandName << ": Utility to check for the existence "
                  << "of a given block inside a given plugin." << std::endl;
        std::cout << "USAGE : " << commandName << " pluginName blockName" << std::endl;
        std::cout << "      : Note that the pluginName should be specified without prefix (lib) or suffix (.dll, .so, .dylib)." << std::endl;

        return EXIT_FAILURE;
    }

    // Get the class name and the library name from the parameter
    const std::string blockName(argv[2]);
    const std::string pluginName(argv[1]);

    // Get the block factory
    auto factory = blockfactory::core::ClassFactorySingleton::getInstance().getClassFactory(
        {pluginName, blockName});

    if (!factory) {
        std::cerr << "ERROR: Failed to get factory object (blockName=" << blockName
                << ",pluginName=" << pluginName << ")";
        return EXIT_FAILURE;
    }

    if (!factory->isValid()) {
        std::cerr << "ERROR: Factory error (" << static_cast<std::uint32_t>(factory->getStatus())
                << "): " << factory->getError().c_str();
        return EXIT_FAILURE;
    }

    std::cout << "SUCCESS: Block \"" << blockName
              << "\" found and loaded from plugin \"" << pluginName << "\"." << std::endl;
    return EXIT_SUCCESS;
}
