# Copyright (C) 2018 Istituto Italiano di Tecnologia (IIT). All rights reserved.
# This software may be modified and distributed under the terms of the
# GNU Lesser General Public License v2.1 or any later version.

set(CODER_HDR
    include/BlockFactory/SimulinkCoder/CoderBlockInformation.h
    include/BlockFactory/SimulinkCoder/GeneratedCodeWrapper.h)

set(CODER_SRC
    src/CoderBlockInformation.cpp)

add_library(SimulinkCoder ${CODER_HDR} ${CODER_SRC})
add_library(BlockFactory::SimulinkCoder ALIAS SimulinkCoder)

set_target_properties(SimulinkCoder PROPERTIES
    VERSION ${PROJECT_VERSION}
    PUBLIC_HEADER "${CODER_HDR}"
    OUTPUT_NAME "BlockFactorySimulinkCoder")

target_link_libraries(SimulinkCoder PUBLIC BlockFactory::Core)
target_include_directories(SimulinkCoder PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_compile_warnings(SimulinkCoder
    WARNINGS_AS_ERRORS ${TREAT_WARNINGS_AS_ERRORS}
    DEPENDS ENABLE_WARNINGS)

# CMake macro and functions
install(
    FILES
    "${CMAKE_SOURCE_DIR}/cmake/TargetFromGeneratedCode.cmake"
    "${CMAKE_SOURCE_DIR}/cmake/SimulinkCoderSetup.cmake"
    DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/BlockFactory/cmake")

install(
    TARGETS SimulinkCoder
    EXPORT BlockFactorySimulinkCoderExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/BlockFactory/SimulinkCoder)

set(EXTRA_CONTENT
    "list(APPEND CMAKE_MODULE_PATH @CMAKE_INSTALL_PREFIX@/@CMAKE_INSTALL_DATAROOTDIR@/BlockFactory/cmake)")

install_basic_package_files(BlockFactorySimulinkCoder
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
    EXPORT BlockFactorySimulinkCoderExport
    FIRST_TARGET SimulinkCoder
    DEPENDENCIES BlockFactoryCore
    NAMESPACE BlockFactory::
    NO_CHECK_REQUIRED_COMPONENTS_MACRO
    INCLUDE_CONTENT ${EXTRA_CONTENT})
