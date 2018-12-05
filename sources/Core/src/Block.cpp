/*
 * Copyright (C) 2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * GNU Lesser General Public License v2.1 or any later version.
 */

#include "BlockFactory/Core/Block.h"
#include "BlockFactory/Core/BlockInformation.h"
#include "BlockFactory/Core/Log.h"
#include "BlockFactory/Core/Parameter.h"

#include <ostream>

using namespace blockfactory::core;

unsigned Block::numberOfParameters()
{
    return Block::NumberOfParameters;
}

std::vector<std::string> Block::additionalBlockOptions()
{
    return {};
}

void Block::parameterAtIndexIsTunable(unsigned /*index*/, bool& tunable)
{
    tunable = false;
}

bool Block::checkParameters(const BlockInformation* /*blockInfo*/)
{
    return true;
}

bool Block::parseParameters(BlockInformation* blockInfo)
{
    if (!blockInfo->addParameterMetadata({ParameterType::STRING, 0, 1, 1, "className"})
        || !blockInfo->addParameterMetadata({ParameterType::STRING, 1, 1, 1, "libName"})) {
        bfError << "Failed to add parameters metadata.";
        return false;
    }

    return blockInfo->parseParameters(m_parameters);
}

bool Block::configureSizeAndPorts(BlockInformation* blockInfo)
{
    if (!Block::parseParameters(blockInfo)) {
        bfError << "Failed to parse Block parameters.";
        return false;
    }

    return true;
}

bool Block::initialize(BlockInformation* blockInfo)
{
    if (!Block::parseParameters(blockInfo)) {
        bfError << "Failed to parse Block parameters.";
        return false;
    }

    return true;
}

unsigned Block::numberOfDiscreteStates()
{
    return 0;
}

unsigned Block::numberOfContinuousStates()
{
    return 0;
}

bool Block::updateDiscreteState(const BlockInformation* /*blockInfo*/)
{
    return true;
}

bool Block::stateDerivative(const BlockInformation* /*blockInfo*/)
{
    return true;
}

bool Block::initializeInitialConditions(const BlockInformation* /*blockInfo*/)
{
    return true;
}

bool Block::terminate(const BlockInformation* /*blockInfo*/)
{
    return true;
}

bool Block::getParameters(blockfactory::core::Parameters& params) const
{
    params = m_parameters;
    return true;
}
