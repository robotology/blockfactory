/*
 * Copyright (C) 2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * GNU Lesser General Public License v2.1 or any later version.
 */

#include <BlockFactory/Core/Block.h>

namespace mock {
    class MockBlock;
}

class mock::MockBlock : public blockfactory::core::Block
{
public:
    MockBlock();
    ~MockBlock() override = default;

    bool output(const blockfactory::core::BlockInformation* blockInfo) override;
};
