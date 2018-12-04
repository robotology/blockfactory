/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "MyMath.h"
#include <shlibpp/SharedLibraryClass.h>

SHLIBPP_DEFINE_SHARED_SUBCLASS(my_math, MyMathImpl, MyMath);

int MyMathImpl::add(int a, int b)
{
    return (a+b);
}

int MyMathImpl::sub(int a, int b)
{
    return (a-b);
}
