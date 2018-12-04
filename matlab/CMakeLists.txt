# Copyright (C) 2018 Istituto Italiano di Tecnologia (IIT). All rights reserved.
# This software may be modified and distributed under the terms of the
# GNU Lesser General Public License v2.1 or any later version.

# Install TLC file
install(
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/BlockFactory.tlc
    DESTINATION ${CMAKE_INSTALL_PREFIX}/mex)
