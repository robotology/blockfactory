# Copyright (C) 2019 Istituto Italiano di Tecnologia (IIT). All rights reserved.
# This software may be modified and distributed under the terms of the
# GNU Lesser General Public License v2.1 or any later version.

include(CMakeParseArguments)

function(target_compile_warnings)
    set(WARNINGS_COMPILE_OPTIONS "")

    set(_options)
    set(_oneValueArgs WARNINGS_AS_ERRORS DEPENDS)
    set(_multiValueArgs)

    set(PREFIX "tcw")
    cmake_parse_arguments(${PREFIX}
        "${_options}"
        "${_oneValueArgs}"
        "${_multiValueArgs}"
        ${ARGN})

    set(_target "${${PREFIX}_UNPARSED_ARGUMENTS}")
    if(NOT _target)
        message(FATAL_ERROR "Target ${_target} does not exist")
    endif()

    set(WARNINGS_COMPILE_OPTIONS "")

    if(${${PREFIX}_WARNINGS_AS_ERRORS})
        set(_warnings_as_errors TRUE)
    else()
        set(_warnings_as_errors FALSE)
    endif()

    if(${${${PREFIX}_DEPENDS}})
        set(_warnings_enabled TRUE)
    else()
        set(_warnings_enabled FALSE)
    endif()

    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        if(${_warnings_as_errors})
            list(APPEND WARNINGS_COMPILE_OPTIONS "/WX")
        endif()
        list(APPEND WARNINGS_COMPILE_OPTIONS "/W4")
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        if(${_warnings_as_errors})
            list(APPEND WARNINGS_COMPILE_OPTIONS "-Werror")
            list(APPEND WARNINGS_COMPILE_OPTIONS "-pedantic-errors")
        endif()
        list(APPEND WARNINGS_COMPILE_OPTIONS "-Wall")
        list(APPEND WARNINGS_COMPILE_OPTIONS "-Wextra")
        list(APPEND WARNINGS_COMPILE_OPTIONS "-Wconversion")
        # Disable some of them
        list(APPEND WARNINGS_COMPILE_OPTIONS "-Wno-sign-compare")
        list(APPEND WARNINGS_COMPILE_OPTIONS "-Wno-unknown-pragmas")
        list(APPEND WARNINGS_COMPILE_OPTIONS "-Wno-sign-conversion")

        if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
            list(APPEND WARNINGS_COMPILE_OPTIONS "-Wno-shorten-64-to-32")
        endif()
    endif()

    if(${_warnings_enabled})
        target_compile_options(${_target} PRIVATE ${WARNINGS_COMPILE_OPTIONS})
    endif()
endfunction()
