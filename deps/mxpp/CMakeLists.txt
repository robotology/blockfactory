# Copyright (C) 2018 Istituto Italiano di Tecnologia (IIT). All rights reserved.
# This software may be modified and distributed under the terms of the
# GNU Lesser General Public License v2.1 or any later version.

cmake_minimum_required(VERSION 3.5)
project(mxpp LANGUAGES CXX VERSION 0.1)

# Configure the project
# =====================

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include(GNUInstallDirs)

# Build position independent code
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Export all symbols in Windows
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# Add a postfix to Windows libraries compiled in debug
if(MSVC)
    set(CMAKE_DEBUG_POSTFIX "d")
endif()

# Control where binaries and libraries are placed in the build folder.
# This simplifies tests running in Windows.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")

# Build the library
# =================

# Add the target
add_library(mxpp
    include/mxpp/MxArray.h
    src/MxArray.cpp)

set_target_properties(mxpp PROPERTIES
        VERSION ${PROJECT_VERSION}
        PUBLIC_HEADER include/mxpp/MxArray.h)

# Find Matlab resources
find_package(Matlab REQUIRED MX_LIBRARY)
target_include_directories(mxpp SYSTEM PRIVATE "${Matlab_INCLUDE_DIRS}")
target_link_libraries(mxpp ${Matlab_MX_LIBRARY})

# Setup the include directories
target_include_directories(mxpp PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

# Build tests
# ===========

# add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)

# Install
# =======

install(TARGETS mxpp
        EXPORT mxppExport
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/mxpp")

include(InstallBasicPackageFiles)
install_basic_package_files(mxpp
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
    EXPORT mxppExport
    NO_CHECK_REQUIRED_COMPONENTS_MACRO)
