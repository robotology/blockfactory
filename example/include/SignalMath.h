/*
 * Copyright (C) 2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * GNU Lesser General Public License v2.1 or any later version.
 */

#ifndef EXAMPLE_SIGNALMATH_H
#define EXAMPLE_SIGNALMATH_H

#include <Core/Block.h>
#include <Core/BlockInformation.h>

#include <memory>
#include <string>

namespace example {
    class SignalMath;
} // namespace example

class example::SignalMath : public wbt::Block
{
private:
    enum class Operation
    {
        ADDITION,
        SUBTRACTION,
        MULTIPLICATION,
    };

    Operation m_operation;

public:
    static const std::string ClassName;

    SignalMath() = default;
    ~SignalMath() override = default;

    unsigned numberOfParameters() override;
    bool parseParameters(wbt::BlockInformation* blockInfo) override;
    bool configureSizeAndPorts(wbt::BlockInformation* blockInfo) override;
    bool initialize(wbt::BlockInformation* blockInfo) override;
    bool output(const wbt::BlockInformation* blockInfo) override;
    bool terminate(const wbt::BlockInformation* blockInfo) override;
};

#endif // EXAMPLE_SIGNALMATH_H
