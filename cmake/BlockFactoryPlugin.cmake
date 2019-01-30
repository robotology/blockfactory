# Copyright (C) 2019 Istituto Italiano di Tecnologia (IIT). All rights reserved.
# This software may be modified and distributed under the terms of the
# GNU Lesser General Public License v2.1 or any later version.

function(register_blockfactory_block)
    set(options)
    set(oneValueArgs BLOCK_NAME PLUGIN_NAME)
    set(multiValueArgs SOURCES HEADERS)

    set(prefix "abb")

    cmake_parse_arguments(${prefix}
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN})

    if(NOT DEFINED ${prefix}_BLOCK_NAME)
        message(FATAL_ERROR "You must specify the name of the block passing BLOCK_NAME")
    endif()

    if(NOT DEFINED ${prefix}_PLUGIN_NAME)
        message(FATAL_ERROR "You must specify the name of the plugin passing PLUGIN_NAME")
    endif()

    set(block_name ${${prefix}_BLOCK_NAME})
    set(plugin_name ${${prefix}_PLUGIN_NAME})

    message(STATUS "Adding \"${block_name}\" block to \"${plugin_name}\" plugin")
    set_property(GLOBAL APPEND PROPERTY ${plugin_name}_HEADERS "${${prefix}_HEADERS}")
    set_property(GLOBAL APPEND PROPERTY ${plugin_name}_SOURCES "${${prefix}_SOURCES}")
endfunction()

function(add_blockfactory_plugin)

    set(options)
    set(oneValueArgs)
    set(multiValueArgs EXTRA_SOURCES)

    set(prefix "abp")

    cmake_parse_arguments(${prefix}
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN})

    set(plugin_name "${${prefix}_UNPARSED_ARGUMENTS}")

    get_property(plugin_headers GLOBAL PROPERTY ${plugin_name}_HEADERS)
    get_property(plugin_sources GLOBAL PROPERTY ${plugin_name}_SOURCES)

    message(STATUS "Creating BlockFactory plugin \"${plugin_name}\"")
    add_library(${plugin_name} SHARED
        "${plugin_headers}"
        "${plugin_sources}"
        "${${prefix}_EXTRA_SOURCES}")

    set_target_properties(${plugin_name} PROPERTIES OUTPUT_NAME "${plugin_name}")
    target_link_libraries(${plugin_name} PUBLIC BlockFactory::Core)
endfunction()

function(install_blockfactory_plugin)

    set(options)
    set(oneValueArgs)
    set(multiValueArgs)

    set(prefix "ibp")

    cmake_parse_arguments(${prefix}
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN})

    set(plugin_name "${${prefix}_UNPARSED_ARGUMENTS}")

    if(NOT TARGET "${plugin_name}")
        message(FATAL_ERROR "Failed to find target for BlockFactory plugin ${${plugin_name}}")
    endif()

    include(GNUInstallDirs)

    if(WIN32)
        message(STATUS "Remember to add \"${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}/blockfactory/\""
                       " to BLOCKFACTORY_PLUGIN_PATH environment variable")
    else()
        message(STATUS "Remember to add \"${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/blockfactory\""
                       " to BLOCKFACTORY_PLUGIN_PATH environment variable")
    endif()

    install(
        TARGETS ${plugin_name}
        EXPORT ${plugin_name}Export
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}/blockfactory"  # Location of the .dll
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}/blockfactory"  # Location of the .lib
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}/blockfactory" # Location of the .so / .dylib
        PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${plugin_name}/Block")
endfunction()

if(NOT ${CMAKE_VERSION} VERSION_LESS 3.13)
    if(NOT ${CMAKE_MINIMUM_REQUIRED_VERSION} VERSION_LESS 3.13)
      message(AUTHOR_WARNING
          "This version of CMake comes with an improved version of target_sources. "
          "Consider to switch the logic substituting global properties.")
    endif()
endif()
