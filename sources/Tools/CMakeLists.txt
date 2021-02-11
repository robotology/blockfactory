# Copyright (C) Istituto Italiano di Tecnologia (IIT). All rights reserved.
# This software may be modified and distributed under the terms of the
# GNU Lesser General Public License v2.1 or any later version.

set(BLOCKFACTORY_EXISTS_SRC
    src/BlockfactoryExists.cpp)

add_executable(blockfactory-exists ${BLOCKFACTORY_EXISTS_SRC})

target_link_libraries(blockfactory-exists PRIVATE BlockFactory::Core)

target_compile_warnings(blockfactory-exists
    WARNINGS_AS_ERRORS ${TREAT_WARNINGS_AS_ERRORS}
    DEPENDS ENABLE_WARNINGS)

install(
    TARGETS blockfactory-exists
    EXPORT BlockFactoryToolsExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

install_basic_package_files(BlockFactoryTools
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
    EXPORT BlockFactoryToolsExport
    FIRST_TARGET blockfactory-exists
    NAMESPACE BlockFactory::
    NO_CHECK_REQUIRED_COMPONENTS_MACRO)
