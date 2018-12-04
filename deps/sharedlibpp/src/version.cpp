/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <shlibpp/version.h>

int shlibpp::getVersionMajor()
{
    return SHLIBPP_VERSION_MAJOR;
}

int shlibpp::getVersionMinor()
{
    return SHLIBPP_VERSION_MINOR;
}

int shlibpp::getVersionPatch()
{
    return SHLIBPP_VERSION_PATCH;
}

std::string shlibpp::getVersion()
{
    return SHLIBPP_VERSION;
}
