# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

cmake_minimum_required(VERSION 3.5)

project(shlibpp
        VERSION 0.0.1
        LANGUAGES CXX)
set(PROJECT_DESCRIPTION "Tiny cross-platform plug-in system (dll, so, dylib)")

# For now keep YCM as a soft dependency
find_package(YCM 0.8.1 QUIET)
if(NOT YCM_FOUND)
  list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
endif()

# Set output directories for targets
include(GNUInstallDirs)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")

# Set postfix for debug builds on MSVC
if(MSVC)
  set(CMAKE_DEBUG_POSTFIX "d")
endif()

# Encourage user to specify a build type (e.g. Release, Debug, etc.), otherwise set it to Release.
if(NOT CMAKE_CONFIGURATION_TYPES)
    if(NOT CMAKE_BUILD_TYPE)
        message(STATUS "Setting build type to 'Release' as none was specified.")
        set_property(CACHE CMAKE_BUILD_TYPE PROPERTY VALUE "Release")
    endif()
endif()

# Shared/Dynamic or Static library?
option(BUILD_SHARED_LIBS "Build libraries as shared as opposed to static" ON)

# Enable RPATH Support
include(AddInstallRPATHSupport)
add_install_rpath_support(BIN_DIRS "${CMAKE_INSTALL_FULL_LIBDIR}"
                          LIB_DIRS "${CMAKE_INSTALL_FULL_BINDIR}"
                          INSTALL_NAME_DIR "${CMAKE_INSTALL_FULL_LIBDIR}"
                          USE_LINK_PATH)

# Build in pure c++11
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build position independent code
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Hide all symbols by default
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

add_subdirectory(src)

# Install CMake config files for the library
include(InstallBasicPackageFiles)
install_basic_package_files(shlibpp
                            VERSION ${shlibpp_VERSION}
                            COMPATIBILITY AnyNewerVersion
                            EXPORT shlibpp
                            NO_CHECK_REQUIRED_COMPONENTS_MACRO)

# Add uninstall target
# include(AddUninstallTarget)

# Do not put tests and examples in the same output directories as the main
# targets
unset(CMAKE_RUNTIME_OUTPUT_DIRECTORY)
unset(CMAKE_LIBRARY_OUTPUT_DIRECTORY)
unset(CMAKE_ARCHIVE_OUTPUT_DIRECTORY)


# Create examples
option(BUILD_SHAREDLIBPP_EXAMPLES "Build examples" OFF)
mark_as_advanced(BUILD_SHAREDLIBPP_EXAMPLES)
if(BUILD_SHAREDLIBPP_EXAMPLES)
  add_subdirectory(examples)
endif()
