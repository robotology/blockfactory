# Copyright (C) 2018 Istituto Italiano di Tecnologia (IIT). All rights reserved.
# This software may be modified and distributed under the terms of the
# GNU Lesser General Public License v2.1 or any later version.

if(USES_MATLAB)
    add_subdirectory(mxpp)
endif()

if(BUILD_TESTING)
    add_subdirectory(catch)
endif()
