# Copyright (C) 2018 Istituto Italiano di Tecnologia (IIT). All rights reserved.
# This software may be modified and distributed under the terms of the
# GNU Lesser General Public License v2.1 or any later version.

add_library(SimulinkBlockInformationPrivate STATIC
    src/SimulinkBlockInformationImpl.cpp
    include/BlockFactory/Mex/Private/SimulinkBlockInformationImpl.h)

target_link_libraries(SimulinkBlockInformationPrivate
    PUBLIC
    BlockFactory::Core
    mxpp)

# Needed to access Simulink C APIs
target_compile_definitions(SimulinkBlockInformationPrivate PRIVATE "MATLAB_MEX_FILE")
target_compile_warnings(SimulinkBlockInformationPrivate
    WARNINGS_AS_ERRORS ${TREAT_WARNINGS_AS_ERRORS}
    DEPENDS ENABLE_WARNINGS)

target_include_directories(SimulinkBlockInformationPrivate PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    ${Matlab_INCLUDE_DIRS}
    ${Matlab_ROOT_DIR}/simulink/include)

# Compile S-Function
matlab_add_mex(
    NAME Mex
    OUTPUT_NAME BlockFactory
    SRC src/SimulinkBlockInformation.cpp
        include/BlockFactory/Mex/SimulinkBlockInformation.h
        src/BlockFactory.cpp
    LINK_TO BlockFactory::Core shlibpp::shlibpp SimulinkBlockInformationPrivate)
add_library(BlockFactory::Mex ALIAS Mex)

# Needed to access Simulink C APIs
target_compile_definitions(Mex PRIVATE "MATLAB_MEX_FILE")
target_compile_warnings(Mex
    WARNINGS_AS_ERRORS ${TREAT_WARNINGS_AS_ERRORS}
    DEPENDS ENABLE_WARNINGS)

target_include_directories(Mex PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    ${Matlab_ROOT_DIR}/simulink/include)

# Install S-Function
install(
    TARGETS Mex SimulinkBlockInformationPrivate
    EXPORT BlockFactoryMexExport
    DESTINATION mex)

install_basic_package_files(BlockFactoryMex
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
    EXPORT BlockFactoryMexExport
    FIRST_TARGET Mex
    DEPENDENCIES BlockFactoryCore mxpp shlibpp
    NAMESPACE BlockFactory::
    NO_CHECK_REQUIRED_COMPONENTS_MACRO)
