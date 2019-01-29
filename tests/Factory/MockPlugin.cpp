/*
 * Copyright (C) 2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * GNU Lesser General Public License v2.1 or any later version.
 */

#include "MockPlugin.h"
#include <BlockFactory/Core/Parameter.h>

#include <shlibpp/SharedLibraryClassApi.h>

mock::MockBlock::MockBlock()
{
    using namespace blockfactory::core;

    ParameterMetadata md(ParameterType::INT, 0, 1, 1, "mockParam");
    m_parameters.storeParameter(42, md);
}

bool mock::MockBlock::output(const blockfactory::core::BlockInformation* /*blockInfo*/) {
    return true;
}

// Add the MockBlock class to the plugin factory
SHLIBPP_DEFINE_SHARED_SUBCLASS(MockBlock, mock::MockBlock, blockfactory::core::Block);
