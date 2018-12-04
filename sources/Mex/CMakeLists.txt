# Copyright (C) 2018 Istituto Italiano di Tecnologia (IIT). All rights reserved.
# This software may be modified and distributed under the terms of the
# GNU Lesser General Public License v2.1 or any later version.

# Compile S-Function
matlab_add_mex(
    NAME Mex
    OUTPUT_NAME BlockFactory
    SRC src/SimulinkBlockInformation.cpp
        include/BlockFactory/Mex/SimulinkBlockInformation.h
        src/BlockFactory.cpp
    LINK_TO BlockFactory::Core MxAnyType shlibpp::shlibpp)
add_library(BlockFactory::Mex ALIAS Mex)

target_compile_definitions(Mex PUBLIC "MATLAB_MEX_FILE")

target_include_directories(Mex PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    ${Matlab_INCLUDE_DIRS}
    ${Matlab_ROOT_DIR}/simulink/include)

# Install S-Function
install(
    TARGETS Mex
    EXPORT BlockFactoryMexExport
    DESTINATION mex)

install_basic_package_files(BlockFactoryMex
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
    EXPORT BlockFactoryMexExport
    FIRST_TARGET Mex
    DEPENDENCIES BlockFactoryCore MxAnyType shlibpp
    NAMESPACE BlockFactory::
    NO_CHECK_REQUIRED_COMPONENTS_MACRO)
