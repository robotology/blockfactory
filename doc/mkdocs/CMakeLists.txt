find_package(PythonInterp)
find_program(MKDOCS_ABSPATH NAMES mkdocs)

include(GNUInstallDirs)

if (PYTHONINTERP_FOUND AND MKDOCS_ABSPATH)
    add_custom_target(mkdocs COMMAND ${MKDOCS_ABSPATH} build -c -s -v --site-dir ${CMAKE_CURRENT_BINARY_DIR}
                             WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                             COMMENT "Generating doxygen documentation in ${DOXYGEN_BUILD_DIR}")
endif()
