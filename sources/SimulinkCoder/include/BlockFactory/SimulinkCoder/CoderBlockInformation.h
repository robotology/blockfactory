/*
 * Copyright (C) 2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * GNU Lesser General Public License v2.1 or any later version.
 */

#ifndef BLOCKFACTORY_CODER_CODERBLOCKINFORMATION_H
#define BLOCKFACTORY_CODER_CODERBLOCKINFORMATION_H

#include "BlockFactory/Core/BlockInformation.h"
#include "BlockFactory/Core/Parameter.h"
#include "BlockFactory/Core/Parameters.h"
#include "BlockFactory/Core/Port.h"
#include "BlockFactory/Core/Signal.h"

#include <memory>
#include <string>

namespace blockfactory {
    namespace coder {
        class CoderBlockInformation;
    } // namespace coder
} // namespace blockfactory

class blockfactory::coder::CoderBlockInformation final : public blockfactory::core::BlockInformation
{
private:
#ifndef DOXYGEN_SHOULD_SKIP_THIS
    class impl;
    std::unique_ptr<impl> pImpl;
#endif

public:
    CoderBlockInformation();
    ~CoderBlockInformation() override;

    bool getUniqueName(std::string& blockUniqueName) const override;

    // BLOCK OPTIONS METHODS
    // =====================

    bool optionFromKey(const std::string& key, double& option) const override;

    // PARAMETERS METHODS
    // ==================

    bool addParameterMetadata(const core::ParameterMetadata& paramMD) override;
    bool parseParameters(core::Parameters& parameters) override;

    // PORT INFORMATION SETTERS
    // ========================

    bool setPortsInfo(const core::InputPortsInfo& inputPortsInfo,
                      const core::OutputPortsInfo& outputPortsInfo) override;

    // PORT INFORMATION GETTERS
    // ========================

    core::Port::Info getInputPortInfo(core::Port::Index idx) const override;
    core::Port::Info getOutputPortInfo(core::Port::Index idx) const override;
    core::Port::Size::Vector getInputPortWidth(const core::Port::Index idx) const override;
    core::Port::Size::Vector getOutputPortWidth(const core::Port::Index idx) const override;
    core::Port::Size::Matrix getInputPortMatrixSize(const core::Port::Index idx) const override;
    core::Port::Size::Matrix getOutputPortMatrixSize(const core::Port::Index idx) const override;

    // BLOCK SIGNALS
    // =============

    core::InputSignalPtr getInputPortSignal(const core::Port::Index idx) const override;
    core::OutputSignalPtr getOutputPortSignal(const core::Port::Index idx) const override;

    // METHODS OUTSIDE THE INTERFACE
    // =============================

    bool setUniqueBlockName(const std::string& blockUniqueName);
    bool storeRTWParameters(const core::Parameters& parameters);
    bool setInputPort(const core::Port::Info& portInfo, void* signalAddress);
    bool setOutputPort(const core::Port::Info& portInfo, void* signalAddress);
};

#endif // BLOCKFACTORY_CODER_CODERBLOCKINFORMATION_H
