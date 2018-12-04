/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef MYMATH_H
#define MYMATH_H

class MyMath
{
public:
    virtual int add(int a, int b) = 0;
    virtual int sub(int a, int b) = 0;
};

class MyMathImpl : public MyMath
{
public:
    int add(int a, int b);
    int sub(int a, int b);
};

#endif //_MYMATH_H_
