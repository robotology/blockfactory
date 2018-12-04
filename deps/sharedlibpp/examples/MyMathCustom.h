/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef MYMATHCUSTOM_H
#define MYMATHCUSTOM_H

#include <cinttypes>

constexpr int32_t CUSTOM_START_CHECK =
    static_cast<int32_t>('T') +
    (static_cast<int32_t>('E') << 8) +
    (static_cast<int32_t>('S') << 16) +
    (static_cast<int32_t>('T') << 24);
constexpr int32_t CUSTOM_END_CHECK =
    static_cast<int32_t>('T') +
    (static_cast<int32_t>('S') << 8) +
    (static_cast<int32_t>('E') << 16) +
    (static_cast<int32_t>('T') << 24);
constexpr int32_t CUSTOM_SYSTEM_VERSION = 42;

class MyMathCustom
{
public:
    virtual int add(int a, int b) = 0;
    virtual int sub(int a, int b) = 0;
};

class MyMathCustomImpl : public MyMathCustom
{
public:
    int add(int a, int b);
    int sub(int a, int b);
};

#endif //_MYMATH_H_
