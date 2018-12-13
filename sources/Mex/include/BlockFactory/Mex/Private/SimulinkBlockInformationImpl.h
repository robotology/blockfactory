/*
 * Copyright (C) 2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * GNU Lesser General Public License v2.1 or any later version.
 */

#ifndef BLOCKFACTORY_MEX_IMPL_SIMULINKBLOCKINFORMATIONIMPL_H
#define BLOCKFACTORY_MEX_IMPL_SIMULINKBLOCKINFORMATIONIMPL_H

#include "AnyType.h"
#include "BlockFactory/Core/BlockInformation.h"

#include <simstruc.h>

#include <string>
#include <vector>

namespace blockfactory {
    namespace mex {
        namespace impl {
            class SimulinkBlockInformationImpl;
        } // namespace impl
    } // namespace mex
} // namespace blockfactory

/**
 * @brief SimulinkBlockInformationImpl provides mainly interfacing with Simulink APIs
 *
 * This class is the private implementation of mex::SimulinkBlockInformation.
 */
class blockfactory::mex::impl::SimulinkBlockInformationImpl
{
private:
public:
    using ParameterIndex = unsigned;

    using DataType = blockfactory::core::DataType;
    using PortData = blockfactory::core::BlockInformation::PortData;
    using PortIndex = blockfactory::core::BlockInformation::PortIndex;
    using VectorSize = blockfactory::core::BlockInformation::VectorSize;
    using MatrixSize = blockfactory::core::BlockInformation::MatrixSize;

    SimStruct* simstruct = nullptr;

    std::string confBlockName;
    std::vector<core::ParameterMetadata> paramsMetadata;

    core::DataType mapSimulinkToPortType(const DTypeId typeId) const;
    DTypeId mapPortTypeToSimulink(const core::DataType dataType) const;

    SimulinkBlockInformationImpl() = delete;
    SimulinkBlockInformationImpl(SimStruct* ss);
    ~SimulinkBlockInformationImpl() = default;

    // =============
    // PORTS METHODS
    // =============

    bool updateInputPortData(const PortData& portData);
    bool updateOutputPortData(const PortData& portData);
    bool setNumberOfInputPorts(const unsigned numberOfPorts);
    bool setNumberOfOutputPorts(const unsigned numberOfPorts);
    bool setInputPortType(const PortIndex idx, const DataType type);
    bool setOutputPortType(const PortIndex idx, const DataType type);
    bool setInputPortVectorSize(const PortIndex idx, const VectorSize& size);
    bool setInputPortMatrixSize(const PortIndex idx, const MatrixSize& size);
    bool setOutputPortVectorSize(const PortIndex idx, const VectorSize& size);
    bool setOutputPortMatrixSize(const PortIndex idx, const MatrixSize& size);

    // =================
    // SCALAR PARAMETERS
    // =================

    bool getScalarParameterAtIndex(const ParameterIndex idx, double& value) const;
    bool getBooleanParameterAtIndex(const ParameterIndex idx, bool& value) const;
    bool getStringParameterAtIndex(const ParameterIndex idx, std::string& value) const;

    // =================================
    // CELL / STRUCT / VECTOR PARAMETERS
    // =================================

    bool getCellAtIndex(const ParameterIndex idx, AnyCell& value) const;
    bool getStructAtIndex(const ParameterIndex idx, AnyStruct& value) const;
    bool getVectorAtIndex(const ParameterIndex idx, std::vector<double>& value) const;

    // ===========================
    // FIELDS OF STRUCT PARAMETERS
    // ===========================

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
};

#endif // BLOCKFACTORY_MEX_IMPL_SIMULINKBLOCKINFORMATIONIMPL_H
