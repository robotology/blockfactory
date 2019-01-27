/*
 * Copyright (C) 2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * GNU Lesser General Public License v2.1 or any later version.
 */

#ifndef BLOCKFACTORY_MEX_SIMULINKBLOCKINFORMATION_H
#define BLOCKFACTORY_MEX_SIMULINKBLOCKINFORMATION_H

#include "BlockFactory/Core/BlockInformation.h"
#include "BlockFactory/Core/Parameter.h"
#include "BlockFactory/Core/Signal.h"

#include <string>
#include <vector>

namespace blockfactory {
    namespace mex {
        class SimulinkBlockInformation;
        namespace impl {
            class SimulinkBlockInformationImpl;
        } // namespace impl
    } // namespace mex
} // namespace blockfactory

// Forward declare SimStruct
typedef struct SimStruct_tag SimStruct;

/**
 * @brief Simulink implementation of block information
 *
 * This class implements the core::BlockInformationfor abstract class providing to core::Block
 * objects the support of interfacing with the Simulink engine.
 */
class blockfactory::mex::SimulinkBlockInformation final : public core::BlockInformation
{
private:
#ifndef DOXYGEN_SHOULD_SKIP_THIS
    std::unique_ptr<impl::SimulinkBlockInformationImpl> pImpl;
#endif
public:
    using ParameterIndex = unsigned;

    SimulinkBlockInformation(SimStruct* simstruct);
    ~SimulinkBlockInformation() override;

    bool optionFromKey(const std::string& key, double& option) const override;
    bool addParameterMetadata(const core::ParameterMetadata& paramMD) override;
    bool parseParameters(core::Parameters& parameters) override;
    bool setIOPortsData(const IOData& ioData) override;
    PortData getInputPortData(const PortIndex idx) const override;
    PortData getOutputPortData(const PortIndex idx) const override;
    VectorSize getInputPortWidth(const PortIndex idx) const override;
    VectorSize getOutputPortWidth(const PortIndex idx) const override;
    MatrixSize getInputPortMatrixSize(const PortIndex idx) const override;
    MatrixSize getOutputPortMatrixSize(const PortIndex idx) const override;
    core::InputSignalPtr
    getInputPortSignal(const PortIndex idx,
                       const VectorSize size = core::Signal::DynamicSize) const override;
    core::OutputSignalPtr
    getOutputPortSignal(const PortIndex idx,
                        const VectorSize size = core::Signal::DynamicSize) const override;
};

#endif /* BLOCKFACTORY_MEX_SIMULINKBLOCKINFORMATION_H */
