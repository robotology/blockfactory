# Copyright (C) 2018 Istituto Italiano di Tecnologia (IIT). All rights reserved.
# This software may be modified and distributed under the terms of the
# GNU Lesser General Public License v2.1 or any later version.

add_library(SimulinkBlockInformationPrivate STATIC
    src/SimulinkBlockInformationImpl.cpp
    include/BlockFactory/Simulink/Private/SimulinkBlockInformationImpl.h)

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
    NAME Simulink
    OUTPUT_NAME BlockFactory
    SRC src/SimulinkBlockInformation.cpp
        include/BlockFactory/Simulink/SimulinkBlockInformation.h
        src/BlockFactory.cpp
    LINK_TO BlockFactory::Core shlibpp::shlibpp SimulinkBlockInformationPrivate)
add_library(BlockFactory::Simulink ALIAS Simulink)

# Needed to access Simulink C APIs
target_compile_definitions(Simulink PRIVATE "MATLAB_MEX_FILE")
target_compile_warnings(Simulink
    WARNINGS_AS_ERRORS ${TREAT_WARNINGS_AS_ERRORS}
    DEPENDS ENABLE_WARNINGS)

# Fix https://github.com/robotology/blockfactory/issues/13
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(Simulink PRIVATE -Wno-format-overflow)
endif()

target_include_directories(Simulink PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    ${Matlab_ROOT_DIR}/simulink/include)

# Install S-Function
install(
    TARGETS Simulink SimulinkBlockInformationPrivate
    EXPORT BlockFactorySimulinkExport
    DESTINATION mex)

install_basic_package_files(BlockFactorySimulink
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
    EXPORT BlockFactorySimulinkExport
    FIRST_TARGET Simulink
    DEPENDENCIES BlockFactoryCore mxpp shlibpp
    NAMESPACE BlockFactory::
    NO_CHECK_REQUIRED_COMPONENTS_MACRO)
