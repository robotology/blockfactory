/*
 * Copyright (C) 2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * GNU Lesser General Public License v2.1 or any later version.
 */

#ifndef WBT_CODERBLOCKINFORMATION_H
#define WBT_CODERBLOCKINFORMATION_H

#include "BlockFactory/Core/BlockInformation.h"
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

    // BLOCK OPTIONS METHODS
    // =====================

    bool optionFromKey(const std::string& key, double& option) const override;

    // PARAMETERS METHODS
    // ==================

    bool addParameterMetadata(const core::ParameterMetadata& paramMD) override;
    bool parseParameters(core::Parameters& parameters) override;

    // PORT INFORMATION SETTERS
    // ========================

    bool setIOPortsData(const IOData& ioData) override;

    // PORT INFORMATION GETTERS
    // ========================

    PortData getInputPortData(PortIndex idx) const override;
    PortData getOutputPortData(PortIndex idx) const override;
    VectorSize getInputPortWidth(const PortIndex idx) const override;
    VectorSize getOutputPortWidth(const PortIndex idx) const override;
    MatrixSize getInputPortMatrixSize(const PortIndex idx) const override;
    MatrixSize getOutputPortMatrixSize(const PortIndex idx) const override;

    // BLOCK SIGNALS
    // =============

    core::InputSignalPtr
    getInputPortSignal(const PortIndex idx,
                       const VectorSize size = core::Signal::DynamicSize) const override;
    core::OutputSignalPtr
    getOutputPortSignal(const PortIndex idx,
                        const VectorSize size = core::Signal::DynamicSize) const override;

    // METHODS OUTSIDE THE INTERFACE
    // =============================

    bool storeRTWParameters(const core::Parameters& parameters);
    bool setInputSignal(const PortIndex idx, void* address, const PortDimension& dims);
    bool setOutputSignal(const PortIndex idx, void* address, const PortDimension& dims);
};

#endif // WBT_CODERBLOCKINFORMATION_H
