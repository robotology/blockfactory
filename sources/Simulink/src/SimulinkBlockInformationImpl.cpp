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

#include <simstruc.h>

using namespace blockfactory::core;
using namespace blockfactory::mex::impl;

SimulinkBlockInformationImpl::SimulinkBlockInformationImpl(SimStruct* ss)
    : simstruct(ss)
{}

DataType SimulinkBlockInformationImpl::mapSimulinkToPortType(const DTypeId typeId) const
{
    switch (typeId) {
        case SS_DOUBLE:
            return core::DataType::DOUBLE;
        case SS_SINGLE:
            return core::DataType::SINGLE;
        case SS_INT8:
            return core::DataType::INT8;
        case SS_UINT8:
            return core::DataType::UINT8;
        case SS_INT16:
            return core::DataType::INT16;
        case SS_UINT16:
            return core::DataType::UINT16;
        case SS_INT32:
            return core::DataType::INT32;
        case SS_UINT32:
            return core::DataType::UINT32;
        case SS_BOOLEAN:
            return core::DataType::BOOLEAN;
        default:
            return core::DataType::DOUBLE;
    }
}

DTypeId SimulinkBlockInformationImpl::mapPortTypeToSimulink(const core::DataType dataType) const
{
    switch (dataType) {
        case core::DataType::DOUBLE:
            return SS_DOUBLE;
        case core::DataType::SINGLE:
            return SS_SINGLE;
        case core::DataType::INT8:
            return SS_INT8;
        case core::DataType::UINT8:
            return SS_UINT8;
        case core::DataType::INT16:
            return SS_INT16;
        case core::DataType::UINT16:
            return SS_UINT16;
        case core::DataType::INT32:
            return SS_INT32;
        case core::DataType::UINT32:
            return SS_UINT32;
        case core::DataType::BOOLEAN:
            return SS_BOOLEAN;
    }

    return {};
}

// =============
// PORTS METHODS
// =============

bool SimulinkBlockInformationImpl::updateInputPortData(const BlockInformation::PortData& portData)
{
    // Get the port dimensions
    const auto& portDimensions = std::get<BlockInformation::Port::Dimensions>(portData);
    if (portDimensions.size() > 2) {
        bfError << "Only vector (1D) and matrix (2D) input ports are supported.";
        return false;
    }

    // Get the port index and type
    const auto& portIndex = std::get<BlockInformation::Port::Index>(portData);
    const auto& portDataType = std::get<BlockInformation::Port::DataType>(portData);

    bool ok = false;

    switch (portDimensions.size()) {
        // 1D Vector
        case 1: {
            const VectorSize width = portDimensions.at(0);
            ok = setInputPortVectorSize(portIndex, width)
                 && setInputPortType(portIndex, portDataType);
            break;
        }
            // 2D Matrix
        case 2: {
            const BlockInformation::Rows rows = portDimensions.at(0);
            const BlockInformation::Cols cols = portDimensions.at(1);
            ok = setInputPortMatrixSize(portIndex, {rows, cols})
                 && setInputPortType(portIndex, portDataType);
            break;
        }
    }

    if (!ok) {
        bfError << "Failed to configure input port with index " << portIndex << ".";
        return false;
    }

    return true;
}

bool SimulinkBlockInformationImpl::updateOutputPortData(const BlockInformation::PortData& portData)
{
    // Get the port dimensions
    const auto portDimensions = std::get<BlockInformation::Port::Dimensions>(portData);
    if (portDimensions.size() > 2) {
        bfError << "Only vector (1D) and matrix (2D) output ports are supported.";
        return false;
    }

    // Get the port index and type
    const auto portIndex = std::get<BlockInformation::Port::Index>(portData);
    const auto portDataType = std::get<BlockInformation::Port::DataType>(portData);

    bool ok = false;

    switch (portDimensions.size()) {
        // 1D Vector
        case 1: {
            const VectorSize width = portDimensions.at(0);
            ok = setOutputPortVectorSize(portIndex, width)
                 && setOutputPortType(portIndex, portDataType);
            break;
        }
            // 2D Matrix
        case 2: {
            const BlockInformation::Rows rows = portDimensions.at(0);
            const BlockInformation::Cols cols = portDimensions.at(1);
            ok = setOutputPortMatrixSize(portIndex, {rows, cols})
                 && setOutputPortType(portIndex, portDataType);
            break;
        }
    }

    if (!ok) {
        bfError << "Failed to configure output port with index " << portIndex << ".";
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

bool SimulinkBlockInformationImpl::setInputPortType(const PortIndex idx, const core::DataType type)
{
    ssSetInputPortDirectFeedThrough(simstruct, idx, 1);
    ssSetInputPortDataType(simstruct, idx, mapPortTypeToSimulink(type));
    return true;
}

bool SimulinkBlockInformationImpl::setOutputPortType(const PortIndex idx, const core::DataType type)
{
    ssSetOutputPortDataType(simstruct, idx, mapPortTypeToSimulink(type));
    return true;
}

bool SimulinkBlockInformationImpl::setInputPortVectorSize(const PortIndex idx,
                                                          const VectorSize& size)
{
    if (size == core::Signal::DynamicSize) {
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
    if (size.first == core::Signal::DynamicSize || size.second == core::Signal::DynamicSize) {
        // TODO: in this case, explore how to use mdlSetOutputPortDimensionInfo and
        // mdlSetDefaultPortDimensionInfo
        ssSetInputPortMatrixDimensions(simstruct, idx, DYNAMICALLY_SIZED, DYNAMICALLY_SIZED);
    }

    return ssSetInputPortMatrixDimensions(simstruct, idx, size.first, size.first);
}

bool SimulinkBlockInformationImpl::setOutputPortVectorSize(const PortIndex idx,
                                                           const VectorSize& size)
{
    if (size == core::Signal::DynamicSize) {
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
    if (size.first == Signal::DynamicSize || size.second == Signal::DynamicSize) {
        // TODO: in this case, explore how to use mdlSetOutputPortDimensionInfo and
        // mdlSetDefaultPortDimensionInfo
        return ssSetOutputPortMatrixDimensions(
            simstruct, idx, DYNAMICALLY_SIZED, DYNAMICALLY_SIZED);
    }

    return ssSetOutputPortMatrixDimensions(simstruct, idx, size.first, size.second);
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
