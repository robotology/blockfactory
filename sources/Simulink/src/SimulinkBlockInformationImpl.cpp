/*
 * Copyright (C) 2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * GNU Lesser General Public License v2.1 or any later version.
 */

#include "BlockFactory/Simulink/Private/SimulinkBlockInformationImpl.h"
#include "BlockFactory/Core/Log.h"
#include "BlockFactory/Core/Parameter.h"
#include "BlockFactory/Core/Signal.h"

#include <cassert>
#include <simstruc.h>

using namespace blockfactory;
using namespace blockfactory::mex::impl;

SimulinkBlockInformationImpl::SimulinkBlockInformationImpl(SimStruct* ss)
    : simstruct(ss)
{}

bool SimulinkBlockInformationImpl::getUniqueName(std::string& blockUniqueName) const
{
    blockUniqueName = ssGetPath(simstruct);
    return true;
}

bool SimulinkBlockInformationImpl::optionFromKey(const std::string& key, double& option) const
{
    if (key == core::BlockOptionPrioritizeOrder) {
        option = SS_OPTION_PLACE_ASAP;
        return true;
    }

    bfError << "Unrecognized block option.";
    return false;
}

core::Port::DataType SimulinkBlockInformationImpl::mapSimulinkToPortType(const DTypeId typeId) const
{
    switch (typeId) {
        case SS_DOUBLE:
            return core::Port::DataType::DOUBLE;
        case SS_SINGLE:
            return core::Port::DataType::SINGLE;
        case SS_INT8:
            return core::Port::DataType::INT8;
        case SS_UINT8:
            return core::Port::DataType::UINT8;
        case SS_INT16:
            return core::Port::DataType::INT16;
        case SS_UINT16:
            return core::Port::DataType::UINT16;
        case SS_INT32:
            return core::Port::DataType::INT32;
        case SS_UINT32:
            return core::Port::DataType::UINT32;
        case SS_BOOLEAN:
            return core::Port::DataType::BOOLEAN;
        default:
            return core::Port::DataType::DOUBLE;
    }
}

DTypeId SimulinkBlockInformationImpl::mapPortTypeToSimulink(const DataType dataType) const
{
    switch (dataType) {
        case core::Port::DataType::DOUBLE:
            return SS_DOUBLE;
        case core::Port::DataType::SINGLE:
            return SS_SINGLE;
        case core::Port::DataType::INT8:
            return SS_INT8;
        case core::Port::DataType::UINT8:
            return SS_UINT8;
        case core::Port::DataType::INT16:
            return SS_INT16;
        case core::Port::DataType::UINT16:
            return SS_UINT16;
        case core::Port::DataType::INT32:
            return SS_INT32;
        case core::Port::DataType::UINT32:
            return SS_UINT32;
        case core::Port::DataType::BOOLEAN:
            return SS_BOOLEAN;
    }

    return {};
}

// =============
// PORTS METHODS
// =============

bool SimulinkBlockInformationImpl::updateInputPortInfo(const PortInfo& portInfo)
{
    if (portInfo.dimension.size() > 2) {
        bfError << "Only vector (1D) and matrix (2D) input ports are supported.";
        return false;
    }

    bool ok = false;

    switch (portInfo.dimension.size()) {
        // 1D Vector
        case 1: {
            const VectorSize width = portInfo.dimension.at(0);
            ok = setInputPortVectorSize(portInfo.index, width)
                 && setInputPortType(portInfo.index, portInfo.dataType);
            break;
        }
            // 2D Matrix
        case 2: {
            const core::Port::Size::Rows rows = portInfo.dimension.at(0);
            const core::Port::Size::Cols cols = portInfo.dimension.at(1);
            ok = setInputPortMatrixSize(portInfo.index, {rows, cols})
                 && setInputPortType(portInfo.index, portInfo.dataType);
            break;
        }
    }

    if (!ok) {
        bfError << "Failed to configure input port with index " << portInfo.index << ".";
        return false;
    }

    return true;
}

bool SimulinkBlockInformationImpl::updateOutputPortInfo(const PortInfo& portInfo)
{
    if (portInfo.dimension.size() > 2) {
        bfError << "Only vector (1D) and matrix (2D) output ports are supported.";
        return false;
    }

    bool ok = false;

    switch (portInfo.dimension.size()) {
        // 1D Vector
        case 1: {
            const VectorSize width = portInfo.dimension.at(0);
            ok = setOutputPortVectorSize(portInfo.index, width)
                 && setOutputPortType(portInfo.index, portInfo.dataType);
            break;
        }
            // 2D Matrix
        case 2: {
            const core::Port::Size::Rows rows = portInfo.dimension.at(0);
            const core::Port::Size::Cols cols = portInfo.dimension.at(1);
            ok = setOutputPortMatrixSize(portInfo.index, {rows, cols})
                 && setOutputPortType(portInfo.index, portInfo.dataType);
            break;
        }
    }

    if (!ok) {
        bfError << "Failed to configure output port with index " << portInfo.index << ".";
        return false;
    }

    return true;
}

bool SimulinkBlockInformationImpl::setNumberOfInputPorts(const size_t numberOfPorts)
{
    return ssSetNumInputPorts(simstruct, static_cast<int>(numberOfPorts));
}

bool SimulinkBlockInformationImpl::setNumberOfOutputPorts(const size_t numberOfPorts)
{
    return ssSetNumOutputPorts(simstruct, static_cast<int>(numberOfPorts));
}

bool SimulinkBlockInformationImpl::setInputPortType(const PortIndex idx, const DataType type)
{
    ssSetInputPortDirectFeedThrough(simstruct, idx, 1);
    ssSetInputPortDataType(simstruct, idx, mapPortTypeToSimulink(type));
    return true;
}

bool SimulinkBlockInformationImpl::setOutputPortType(const PortIndex idx, const DataType type)
{
    ssSetOutputPortDataType(simstruct, idx, mapPortTypeToSimulink(type));
    return true;
}

bool SimulinkBlockInformationImpl::setInputPortVectorSize(const PortIndex idx,
                                                          const VectorSize& size)
{
    if (size == core::Port::DynamicSize) {
        // TODO: in this case, explore how to use mdlSetOutputPortDimensionInfo and
        // mdlSetDefaultPortDimensionInfo
        return ssSetInputPortVectorDimension(simstruct, idx, DYNAMICALLY_SIZED);
    }

    return ssSetInputPortVectorDimension(simstruct, idx, size);
}

bool SimulinkBlockInformationImpl::setInputPortMatrixSize(const PortIndex idx,
                                                          const MatrixSize& size)
{
    // Refer to: https://it.mathworks.com/help/simulink/sfg/sssetoutputportmatrixdimensions.html
    if (size.rows == core::Port::DynamicSize || size.cols == core::Port::DynamicSize) {
        // TODO: in this case, explore how to use mdlSetOutputPortDimensionInfo and
        // mdlSetDefaultPortDimensionInfo
        ssSetInputPortMatrixDimensions(simstruct, idx, DYNAMICALLY_SIZED, DYNAMICALLY_SIZED);
    }

    return ssSetInputPortMatrixDimensions(simstruct, idx, size.rows, size.cols);
}

bool SimulinkBlockInformationImpl::setOutputPortVectorSize(const PortIndex idx,
                                                           const VectorSize& size)
{
    if (size == core::Port::DynamicSize) {
        // TODO: in this case, explore how to use mdlSetOutputPortDimensionInfo and
        // mdlSetDefaultPortDimensionInfo
        return ssSetOutputPortVectorDimension(simstruct, idx, DYNAMICALLY_SIZED);
    }

    return ssSetOutputPortVectorDimension(simstruct, idx, size);
}

bool SimulinkBlockInformationImpl::setOutputPortMatrixSize(const PortIndex idx,
                                                           const MatrixSize& size)
{
    // Refer to: https://it.mathworks.com/help/simulink/sfg/sssetinputportmatrixdimensions.html
    if (size.rows == core::Port::DynamicSize || size.cols == core::Port::DynamicSize) {
        // TODO: in this case, explore how to use mdlSetOutputPortDimensionInfo and
        // mdlSetDefaultPortDimensionInfo
        return ssSetOutputPortMatrixDimensions(
            simstruct, idx, DYNAMICALLY_SIZED, DYNAMICALLY_SIZED);
    }

    return ssSetOutputPortMatrixDimensions(simstruct, idx, size.rows, size.cols);
}

size_t SimulinkBlockInformationImpl::getNrOfInputPortElements(const PortIndex idx) const
{
    PortInfo portInfo = getInputPortInfo(idx);

    size_t nrOfElements = 1;
    for (int dim : portInfo.dimension) {
        dim == core::Port::DynamicSize ? dim = 0 : true;
        nrOfElements *= dim;
    }

    return nrOfElements;
}

size_t SimulinkBlockInformationImpl::getNrOfOutputPortElements(const PortIndex idx) const
{
    PortInfo portInfo = getOutputPortInfo(idx);

    size_t nrOfElements = 1;
    for (int dim : portInfo.dimension) {
        dim == core::Port::DynamicSize ? dim = 0 : true;
        nrOfElements *= dim;
    }

    return nrOfElements;
}

SimulinkBlockInformationImpl::PortInfo
SimulinkBlockInformationImpl::getInputPortInfo(const PortIndex idx) const
{
    core::Port::Dimensions portDimension;

    const DataType dt = mapSimulinkToPortType(ssGetInputPortDataType(simstruct, idx));

    switch (ssGetInputPortNumDimensions(simstruct, idx)) {
        case 1: {
            auto width = ssGetInputPortWidth(simstruct, idx);
            width == DYNAMICALLY_SIZED ? width = core::Port::DynamicSize : true;
            portDimension = {width};
            break;
        }
        case 2: {
            auto dims = ssGetInputPortDimensions(simstruct, idx);
            dims[0] == DYNAMICALLY_SIZED ? dims[0] = core::Port::DynamicSize : true;
            dims[1] == DYNAMICALLY_SIZED ? dims[1] = core::Port::DynamicSize : true;
            portDimension = {dims[0], dims[1]};
            break;
        }
        default:
            bfError << "Unsupported number of port dimensions for port at index " << idx;
            assert(false);
    }

    return {idx, portDimension, dt};
}

SimulinkBlockInformationImpl::PortInfo
SimulinkBlockInformationImpl::getOutputPortInfo(const PortIndex idx) const
{
    core::Port::Dimensions portDimension;

    const DataType dt = mapSimulinkToPortType(ssGetOutputPortDataType(simstruct, idx));

    switch (ssGetOutputPortNumDimensions(simstruct, idx)) {
        case 1: {
            auto width = ssGetOutputPortWidth(simstruct, idx);
            width == DYNAMICALLY_SIZED ? width = core::Port::DynamicSize : true;
            portDimension = {width};
            break;
        }
        case 2: {
            auto dims = ssGetOutputPortDimensions(simstruct, idx);
            dims[0] == DYNAMICALLY_SIZED ? dims[0] = core::Port::DynamicSize : true;
            dims[1] == DYNAMICALLY_SIZED ? dims[1] = core::Port::DynamicSize : true;
            portDimension = {dims[0], dims[1]};
            break;
        }
        default:
            bfError << "Unsupported number of port dimensions for port at index " << idx;
            assert(false);
    }

    return {idx, portDimension, dt};
}

bool SimulinkBlockInformationImpl::isInputPortDynamicallySized(const PortIndex idx) const
{
    PortInfo portInfo = getInputPortInfo(idx);

    for (auto dim : portInfo.dimension) {
        if (dim == core::Port::DynamicSize) {
            return true;
        }
    }

    return false;
}

bool SimulinkBlockInformationImpl::isOutputPortDynamicallySized(const PortIndex idx) const
{
    PortInfo portInfo = getOutputPortInfo(idx);
    for (auto dim : portInfo.dimension) {
        if (dim == core::Port::DynamicSize) {
            return true;
        }
    }

    return false;
}

bool SimulinkBlockInformationImpl::isInputSignalAtIdxContiguous(
    const SimulinkBlockInformationImpl::PortIndex idx) const
{
    return ssGetInputPortRequiredContiguous(simstruct, idx);
}

ContiguousInputSignalRawPtr
SimulinkBlockInformationImpl::getContiguousSignalRawPtrFromInputPort(const PortIndex idx) const
{
    auto ptr = ssGetInputPortSignal(simstruct, idx);
    return idx >= ssGetNumInputPorts(simstruct) ? nullptr : ptr;
}

NonContiguousInputSignalRawPtr
SimulinkBlockInformationImpl::getNonContiguousSignalRawPtrFromInputPort(const PortIndex idx) const
{
    auto ptr = ssGetInputPortSignalPtrs(simstruct, idx);
    return idx >= ssGetNumInputPorts(simstruct) ? nullptr : ptr;
}

ContiguousOutputSignalRawPtr
SimulinkBlockInformationImpl::getSignalRawPtrFromOutputPort(const PortIndex idx) const
{
    auto ptr = ssGetOutputPortSignal(simstruct, idx);
    return idx >= ssGetNumOutputPorts(simstruct) ? nullptr : ptr;
}

// =================
// SCALAR PARAMETERS
// =================

bool SimulinkBlockInformationImpl::getScalarParameterAtIndex(const ParameterIndex idx,
                                                             double& value) const
{
    const mxArray* blockParam = ssGetSFcnParam(simstruct, idx);
    return mxpp::MxArray(blockParam).asDouble(value);
}

bool SimulinkBlockInformationImpl::getBooleanParameterAtIndex(const ParameterIndex idx,
                                                              bool& value) const
{
    double tmpValue = 0;
    const mxArray* blockParam = ssGetSFcnParam(simstruct, idx);

    // The Simulink mask often doesn't store boolean data from the mask as bool but as double.
    // Calling asBool() will fail in this case. If this happens, asDouble() is used as fallback.
    if (mxpp::MxArray(blockParam).asBool(value)) {
        return true;
    }
    else if (mxpp::MxArray(blockParam).asDouble(tmpValue)) {
        value = static_cast<bool>(tmpValue);
        return true;
    }

    bfError << "Failed to parse bool parameter";
    return false;
}

bool SimulinkBlockInformationImpl::getStringParameterAtIndex(const ParameterIndex idx,
                                                             std::string& value) const
{
    const mxArray* blockParam = ssGetSFcnParam(simstruct, idx);
    return mxpp::MxArray(blockParam).asString(value);
}

// =================================
// CELL / STRUCT / VECTOR PARAMETERS
// =================================

bool SimulinkBlockInformationImpl::getCellAtIndex(const ParameterIndex idx,
                                                  mxpp::MxCell& value) const
{
    const mxArray* blockParam = ssGetSFcnParam(simstruct, idx);
    return mxpp::MxArray(blockParam).asMxCell(value);
}

bool SimulinkBlockInformationImpl::getStructAtIndex(const ParameterIndex idx,
                                                    mxpp::MxStruct& value) const
{
    const mxArray* blockParam = ssGetSFcnParam(simstruct, idx);
    return mxpp::MxArray(blockParam).asMxStruct(value);
}

bool SimulinkBlockInformationImpl::getVectorAtIndex(const ParameterIndex idx,
                                                    std::vector<double>& value) const
{
    const mxArray* blockParam = ssGetSFcnParam(simstruct, idx);
    return mxpp::MxArray(blockParam).asVectorDouble(value);
}

// ===========================
// FIELDS OF STRUCT PARAMETERS
// ===========================

bool SimulinkBlockInformationImpl::getStringFieldAtIndex(const ParameterIndex idx,
                                                         const std::string& fieldName,
                                                         std::string& value) const
{
    mxpp::MxStruct s;

    if (!getStructAtIndex(idx, s)) {
        bfError << "Failed to get struct at index " << idx << ".";
        return false;
    }

    if (s.find(fieldName) == s.end()) {
        bfError << "Struct at index " << idx << " does not contain any " << fieldName << " field.";
        return false;
    }

    return s.at(fieldName)->asString(value);
}

bool SimulinkBlockInformationImpl::getScalarFieldAtIndex(const ParameterIndex idx,
                                                         const std::string& fieldName,
                                                         double& value) const
{
    mxpp::MxStruct s;

    if (!getStructAtIndex(idx, s)) {
        bfError << "Failed to get struct at index " << idx << ".";
        return false;
    }

    if (s.find(fieldName) == s.end()) {
        bfError << "Struct at index " << idx << " does not contain any " << fieldName << " field.";
        return false;
    }

    return s.at(fieldName)->asDouble(value);
}

bool SimulinkBlockInformationImpl::getBooleanFieldAtIndex(const ParameterIndex idx,
                                                          const std::string& fieldName,
                                                          bool& value) const
{
    mxpp::MxStruct s;

    if (!getStructAtIndex(idx, s)) {
        bfError << "Failed to get struct at index " << idx << ".";
        return false;
    }

    if (s.find(fieldName) == s.end()) {
        bfError << "Struct at index " << idx << " does not contain any " << fieldName << " field.";
        return false;
    }

    return s.at(fieldName)->asBool(value);
}

bool SimulinkBlockInformationImpl::getCellFieldAtIndex(const ParameterIndex idx,
                                                       const std::string& fieldName,
                                                       mxpp::MxCell& value) const
{
    mxpp::MxStruct s;

    if (!getStructAtIndex(idx, s)) {
        bfError << "Failed to get struct at index " << idx << ".";
        return false;
    }

    if (s.find(fieldName) == s.end()) {
        bfError << "Struct at index " << idx << " does not contain any " << fieldName << " field.";
        return false;
    }

    return s.at(fieldName)->asMxCell(value);
}

bool SimulinkBlockInformationImpl::getVectorDoubleFieldAtIndex(const ParameterIndex idx,
                                                               const std::string& fieldName,
                                                               std::vector<double>& value) const
{
    mxpp::MxStruct s;

    if (!getStructAtIndex(idx, s)) {
        bfError << "Failed to get struct at index " << idx << ".";
        return false;
    }

    if (s.find(fieldName) == s.end()) {
        bfError << "Struct at index " << idx << " does not contain any " << fieldName << " field.";
        return false;
    }

    return s.at(fieldName)->asVectorDouble(value);
}
