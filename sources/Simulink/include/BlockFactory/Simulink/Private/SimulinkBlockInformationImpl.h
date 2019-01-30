/*
 * Copyright (C) 2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * GNU Lesser General Public License v2.1 or any later version.
 */

#ifndef BLOCKFACTORY_MEX_IMPL_SIMULINKBLOCKINFORMATIONIMPL_H
#define BLOCKFACTORY_MEX_IMPL_SIMULINKBLOCKINFORMATIONIMPL_H

#include "BlockFactory/Core/BlockInformation.h"
#include "mxpp/MxArray.h"

#include <simstruc.h>

#include <string>
#include <vector>

namespace blockfactory {
    namespace mex {
        namespace impl {
            using ContiguousInputSignalRawPtr = const void*;
            using NonContiguousInputSignalRawPtr = InputPtrsType;
            using ContiguousOutputSignalRawPtr = void*;
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

    using DataType = blockfactory::core::Port::DataType;
    using PortInfo = blockfactory::core::Port::Info;
    using PortIndex = blockfactory::core::Port::Index;
    using VectorSize = blockfactory::core::Port::Size::Vector;
    using MatrixSize = blockfactory::core::Port::Size::Matrix;

    SimStruct* simstruct = nullptr;

    std::string confBlockName;
    std::vector<core::ParameterMetadata> paramsMetadata;

    DataType mapSimulinkToPortType(const DTypeId typeId) const;
    DTypeId mapPortTypeToSimulink(const DataType dataType) const;

    SimulinkBlockInformationImpl() = delete;
    SimulinkBlockInformationImpl(SimStruct* ss);
    ~SimulinkBlockInformationImpl() = default;

    bool getUniqueName(std::string& blockUniqueName) const;

    // =====================
    // BLOCK OPTIONS METHODS
    // =====================

    bool optionFromKey(const std::string& key, double& option) const;

    // =============
    // PORTS METHODS
    // =============

    bool updateInputPortInfo(const PortInfo& portData);
    bool updateOutputPortInfo(const PortInfo& portData);
    bool setNumberOfInputPorts(const size_t numberOfPorts);
    bool setNumberOfOutputPorts(const size_t numberOfPorts);
    bool setInputPortType(const PortIndex idx, const DataType type);
    bool setOutputPortType(const PortIndex idx, const DataType type);
    bool setInputPortVectorSize(const PortIndex idx, const VectorSize& size);
    bool setInputPortMatrixSize(const PortIndex idx, const MatrixSize& size);
    bool setOutputPortVectorSize(const PortIndex idx, const VectorSize& size);
    bool setOutputPortMatrixSize(const PortIndex idx, const MatrixSize& size);
    size_t getNrOfInputPortElements(const PortIndex idx) const;
    size_t getNrOfOutputPortElements(const PortIndex idx) const;
    PortInfo getInputPortInfo(const PortIndex idx) const;
    PortInfo getOutputPortInfo(const PortIndex idx) const;
    bool isInputPortDynamicallySized(const PortIndex idx) const;
    bool isOutputPortDynamicallySized(const PortIndex idx) const;

    // ===============
    // SIGNALS METHODS
    // ===============

    bool isInputSignalAtIdxContiguous(const PortIndex idx) const;
    ContiguousInputSignalRawPtr getContiguousSignalRawPtrFromInputPort(const PortIndex idx) const;
    NonContiguousInputSignalRawPtr
    getNonContiguousSignalRawPtrFromInputPort(const PortIndex idx) const;
    ContiguousOutputSignalRawPtr getSignalRawPtrFromOutputPort(const PortIndex idx) const;

    // =================
    // SCALAR PARAMETERS
    // =================

    bool getScalarParameterAtIndex(const ParameterIndex idx, double& value) const;
    bool getBooleanParameterAtIndex(const ParameterIndex idx, bool& value) const;
    bool getStringParameterAtIndex(const ParameterIndex idx, std::string& value) const;

    // =================================
    // CELL / STRUCT / VECTOR PARAMETERS
    // =================================

    bool getCellAtIndex(const ParameterIndex idx, mxpp::MxCell& value) const;
    bool getStructAtIndex(const ParameterIndex idx, mxpp::MxStruct& value) const;
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
                             mxpp::MxCell& value) const;
    bool getVectorDoubleFieldAtIndex(const ParameterIndex idx,
                                     const std::string& fieldName,
                                     std::vector<double>& value) const;
};

#endif // BLOCKFACTORY_MEX_IMPL_SIMULINKBLOCKINFORMATIONIMPL_H
