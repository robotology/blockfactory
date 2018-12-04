# Copyright (C) 2018 Istituto Italiano di Tecnologia (IIT). All rights reserved.
# This software may be modified and distributed under the terms of the
# GNU Lesser General Public License v2.1 or any later version.

add_subdirectory(Core)
add_subdirectory(Coder)

if(USES_MATLAB)
    add_subdirectory(Mex)
endif()

# ========================
# MAIN BLOCKFACTORY TARGET
# ========================

# Dummy target
add_library(BlockFactory INTERFACE)

set(BLOCKFACTORY_DEPENDENCIES BlockFactoryCore BlockFactoryCoder)
if (USES_MATLAB)
    list(APPEND BLOCKFACTORY_DEPENDENCIES BlockFactoryMex)
endif()

install(
    TARGETS BlockFactory
    EXPORT BlockFactoryExport)

install_basic_package_files(BlockFactory
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
    EXPORT BlockFactoryExport
    DEPENDENCIES ${BLOCKFACTORY_DEPENDENCIES}
    NAMESPACE BlockFactory::
    INCLUDE_FILE ${CMAKE_SOURCE_DIR}/cmake/ExtraPackageConfigVars.cmake.in)
