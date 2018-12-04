# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

include (GNUInstallDirs)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/shlibpp/config.h.in"
               "${CMAKE_CURRENT_BINARY_DIR}/shlibpp/config.h"
               @ONLY)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/shlibpp/version.h.in"
               "${CMAKE_CURRENT_BINARY_DIR}/shlibpp/version.h"
               @ONLY)

set(shlibpp_HDRS "${CMAKE_CURRENT_BINARY_DIR}/shlibpp/config.h"
                 "${CMAKE_CURRENT_BINARY_DIR}/shlibpp/version.h"
                 shlibpp/api.h
                 shlibpp/SharedLibraryClassApi.h
                 shlibpp/SharedLibraryClassFactory.h
                 shlibpp/SharedLibraryClassFactory-inl.h
                 shlibpp/SharedLibraryClass.h
                 shlibpp/SharedLibraryClass-inl.h
                 shlibpp/SharedLibraryFactory.h
                 shlibpp/SharedLibrary.h)

set(shlibpp_SRCS version.cpp
                 SharedLibrary.cpp
                 SharedLibraryFactory.cpp)

add_library(shlibpp ${shlibpp_SRCS} ${shlibpp_HDRS})
add_library(shlibpp::shlibpp ALIAS shlibpp)

# Add build definitions
if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(shlibpp PRIVATE SHLIBPP_STATIC)
endif()
set_target_properties(shlibpp PROPERTIES DEFINE_SYMBOL BUILDING_SHLIBPP)

target_include_directories(shlibpp PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                                          $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
                                          $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

if(NOT CMAKE_MINIMUM_REQUIRED_VERSION VERSION_LESS 3.8)
  message(AUTHOR_WARNING "CMAKE_MINIMUM_REQUIRED_VERSION is now ${CMAKE_MINIMUM_REQUIRED_VERSION}. This check can be removed")
endif()
if(CMAKE_VERSION VERSION_LESS 3.8)
  # Should be enough to enable c++11
  target_compile_features(shlibpp PUBLIC cxx_constexpr
                                         cxx_nullptr)
else()
  target_compile_features(shlibpp PUBLIC cxx_std_11)
endif()

if(UNIX)
  target_link_libraries(shlibpp PRIVATE dl)
endif()

set_property(TARGET shlibpp PROPERTY PUBLIC_HEADER ${shlibpp_HDRS})
set_property(TARGET shlibpp PROPERTY VERSION ${shlibpp_VERSION})
set_property(TARGET shlibpp PROPERTY SOVERSION 1)

install(TARGETS shlibpp
        EXPORT shlibpp
        COMPONENT shlibpp
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/shlibpp)
