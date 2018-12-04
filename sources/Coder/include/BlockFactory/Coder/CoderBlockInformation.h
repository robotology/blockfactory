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

namespace wbt {
    class CoderBlockInformation;
}

class wbt::CoderBlockInformation final : public wbt::BlockInformation
{
private:
    class impl;
    std::unique_ptr<impl> pImpl;

public:
    CoderBlockInformation();
    ~CoderBlockInformation() override;

    // BLOCK OPTIONS METHODS
    // =====================

    bool optionFromKey(const std::string& key, double& option) const override;

    // PARAMETERS METHODS
    // ==================

    bool addParameterMetadata(const wbt::ParameterMetadata& paramMD) override;
    bool parseParameters(wbt::Parameters& parameters) override;

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

    wbt::InputSignalPtr
    getInputPortSignal(const PortIndex idx,
                       const VectorSize size = wbt::Signal::DynamicSize) const override;
    wbt::OutputSignalPtr
    getOutputPortSignal(const PortIndex idx,
                        const VectorSize size = wbt::Signal::DynamicSize) const override;

    // METHODS OUTSIDE THE INTERFACE
    // =============================

    bool storeRTWParameters(const Parameters& parameters);
    bool setInputSignal(const PortIndex idx, void* address, const PortDimension& dims);
    bool setOutputSignal(const PortIndex idx, void* address, const PortDimension& dims);
};

#endif // WBT_CODERBLOCKINFORMATION_H
