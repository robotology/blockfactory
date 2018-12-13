/*
 * Copyright (C) 2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * GNU Lesser General Public License v2.1 or any later version.
 */

#ifndef BLOCKFACTORY_MEX_SIMULINKBLOCKINFORMATION_H
#define BLOCKFACTORY_MEX_SIMULINKBLOCKINFORMATION_H

#include "AnyType.h"
#include "BlockFactory/Core/BlockInformation.h"
#include "BlockFactory/Core/Parameter.h"
#include "BlockFactory/Core/Signal.h"

// This is a typedef. We could forward declare it but since it might
// change it is better keeping this include here.
#include <simstruc.h>

#include <string>
#include <vector>

namespace blockfactory {
    namespace mex {
        class SimulinkBlockInformation;
    } // namespace mex
} // namespace blockfactory

class blockfactory::mex::SimulinkBlockInformation final : public core::BlockInformation
{
private:
#ifndef DOXYGEN_SHOULD_SKIP_THIS
    class Impl;
    std::unique_ptr<Impl> pImpl;
#endif
public:
    using ParameterIndex = unsigned;

    SimulinkBlockInformation(SimStruct* simstruct);
    ~SimulinkBlockInformation() override;

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

    PortData getInputPortData(const PortIndex idx) const override;
    PortData getOutputPortData(const PortIndex idx) const override;
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

    // Ports methods
    bool updateInputPortData(const PortData& portData);
    bool updateOutputPortData(const PortData& portData);
    bool setNumberOfInputPorts(const unsigned numberOfPorts);
    bool setNumberOfOutputPorts(const unsigned numberOfPorts);
    bool setInputPortType(const PortIndex idx, const core::DataType type);
    bool setOutputPortType(const PortIndex idx, const core::DataType type);
    bool setInputPortVectorSize(const PortIndex idx, const VectorSize& size);
    bool setInputPortMatrixSize(const PortIndex idx, const MatrixSize& size);
    bool setOutputPortVectorSize(const PortIndex idx, const VectorSize& size);
    bool setOutputPortMatrixSize(const PortIndex idx, const MatrixSize& size);

    // Scalar parameters
    bool getScalarParameterAtIndex(const ParameterIndex idx, double& value) const;
    bool getBooleanParameterAtIndex(const ParameterIndex idx, bool& value) const;
    bool getStringParameterAtIndex(const ParameterIndex idx, std::string& value) const;

    // Struct parameters
    bool getStringFieldAtIndex(const ParameterIndex idx,
                               const std::string& fieldName,
                               std::string& value) const;
    bool getScalarFieldAtIndex(const ParameterIndex idx,
                               const std::string& fieldName,
                               double& value) const;
    bool getBooleanFieldAtIndex(const ParameterIndex idx,
                                const std::string& fieldName,
                                bool& value) const;
    bool getCellFieldAtIndex(const ParameterIndex idx,
                             const std::string& fieldName,
                             AnyCell& value) const;
    bool getVectorDoubleFieldAtIndex(const ParameterIndex idx,
                                     const std::string& fieldName,
                                     std::vector<double>& value) const;

    // Cell / Struct / Vector paramters
    bool getCellAtIndex(const ParameterIndex idx, AnyCell& value) const;
    bool getStructAtIndex(const ParameterIndex idx, AnyStruct& value) const;
    bool getVectorAtIndex(const ParameterIndex idx, std::vector<double>& value) const;
};

#endif /* BLOCKFACTORY_MEX_SIMULINKBLOCKINFORMATION_H */
