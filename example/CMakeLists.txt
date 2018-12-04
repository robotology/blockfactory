cmake_minimum_required(VERSION 3.5)
project(ExampleToolbox LANGUAGES CXX VERSION 0.1)

# C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Export all symbols in Windows
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# Utility for defining install folders
include(GNUInstallDirs)

# Tweak linker flags in Linux.
# Matlab is very strict on missing symbols and by default ld do not warn if
# something is missing.
if(UNIX AND NOT APPLE)
    get_filename_component(LINKER_BIN ${CMAKE_LINKER} NAME)
    if(${LINKER_BIN} STREQUAL "ld")
        set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--unresolved-symbols=report-all")
    endif()
endif()

# ===========
# C++ LIBRARY
# ===========

# Find the needed WBToolbox components:
# - ToolboxCore contains the core classes such as Block and Signal
# - ToolboxMex is required at runtime for loading the library from Simulink
find_package(WBToolbox 4 REQUIRED COMPONENTS ToolboxCore ToolboxMex)

# Find the project for the multiplatform support of plugin libraries.
# It is shipped with WBToolbox and it should be already installed in your system.
find_package(shlibpp REQUIRED)

# Create the plugin library. This must be a SHARED library.
add_library(ExampleToolbox SHARED
    include/SignalMath.h
    src/SignalMath.cpp
    src/Factory.cpp)

# Manually set the name of the output library. This is not required and it
# is done only for sake of clarity.
set_target_properties(ExampleToolbox PROPERTIES
    OUTPUT_NAME "ExampleToolbox")

# Link the library with the Core component containing the core classes
# and the target that provides the plugin support
target_link_libraries(ExampleToolbox
    WBToolbox::Core shlibpp::shlibpp)

# Setup the include directories
target_include_directories(ExampleToolbox PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/ExampleToolbox>)
