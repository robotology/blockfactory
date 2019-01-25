# Copyright (C) 2019 Istituto Italiano di Tecnologia (IIT). All rights reserved.
# This software may be modified and distributed under the terms of the
# GNU Lesser General Public License v2.1 or any later version.

# Allow setting from the outside the Matlab root dir
if(NOT Matlab_ROOT_DIR)
    find_package(Matlab REQUIRED)
endif()

if(NOT TARGET SimulinkCoder)
    add_library(SimulinkCoder INTERFACE)
    target_include_directories(SimulinkCoder SYSTEM INTERFACE
        "${Matlab_ROOT_DIR}/extern/include"
        "${Matlab_ROOT_DIR}/simulink/include"
        "${Matlab_ROOT_DIR}/rtw/c/include"
        "${Matlab_ROOT_DIR}/rtw/c/src"
        "${Matlab_ROOT_DIR}/rtw/c/src/ext_mode/common")
endif()
