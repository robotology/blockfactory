# Copyright (C) 2018 Istituto Italiano di Tecnologia (IIT). All rights reserved.
# This software may be modified and distributed under the terms of the
# GNU Lesser General Public License v2.1 or any later version.

set(CORE_SRC
    src/Block.cpp
    src/BlockInformation.cpp
    src/Log.cpp
    src/Parameter.cpp
    src/Parameters.cpp
    src/ConvertStdVector.cpp
    src/Signal.cpp
    src/FactorySingleton.cpp)

set(CORE_PUBLIC_HDR
    include/BlockFactory/Core/Block.h
    include/BlockFactory/Core/BlockInformation.h
    include/BlockFactory/Core/Log.h
    include/BlockFactory/Core/Parameter.h
    include/BlockFactory/Core/Parameters.h
    include/BlockFactory/Core/Signal.h
    include/BlockFactory/Core/FactorySingleton.h)

set(CORE_PRIVATE_HDR
    include/BlockFactory/Core/ConvertStdVector.h)

add_library(Core ${CORE_SRC} ${CORE_PUBLIC_HDR} ${CORE_PRIVATE_HDR})
add_library(BlockFactory::Core ALIAS Core)

target_link_libraries(Core PUBLIC shlibpp::shlibpp)

target_include_directories(Core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

set_target_properties(Core PROPERTIES
    VERSION ${PROJECT_VERSION}
    PUBLIC_HEADER "${CORE_PUBLIC_HDR}"
    OUTPUT_NAME "BlockFactoryCore")

target_compile_warnings(Core
    WARNINGS_AS_ERRORS ${TREAT_WARNINGS_AS_ERRORS}
    DEPENDS ENABLE_WARNINGS)

install(
    TARGETS Core
    EXPORT BlockFactoryCoreExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/BlockFactory/Core)

install_basic_package_files(BlockFactoryCore
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
    EXPORT BlockFactoryCoreExport
    FIRST_TARGET Core
    DEPENDENCIES shlibpp
    NAMESPACE BlockFactory::
    NO_CHECK_REQUIRED_COMPONENTS_MACRO)
