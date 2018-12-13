/*
 * Copyright (C) 2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * GNU Lesser General Public License v2.1 or any later version.
 */

#include "BlockFactory/Mex/SimulinkBlockInformation.h"
#include "BlockFactory/Core/Log.h"
#include "BlockFactory/Core/Parameter.h"
#include "BlockFactory/Core/Parameters.h"
#include "BlockFactory/Core/Signal.h"
#include "MxAnyType.h"

#include <memory>
#include <ostream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

using namespace blockfactory;
using namespace blockfactory::mex;

class SimulinkBlockInformation::Impl
{
public:
    SimStruct* simstruct = nullptr;

    std::string m_confBlockName;
    std::vector<core::ParameterMetadata> m_paramsMetadata;

    core::DataType mapSimulinkToPortType(const DTypeId typeId) const;
    DTypeId mapPortTypeToSimulink(const core::DataType dataType) const;
};

SimulinkBlockInformation::SimulinkBlockInformation(SimStruct* S)
    : pImpl{new Impl()}
{
    pImpl->simstruct = S;
}

SimulinkBlockInformation::~SimulinkBlockInformation() = default;

// BLOCK OPTIONS METHODS
// =====================

bool SimulinkBlockInformation::optionFromKey(const std::string& key, double& option) const
{
    if (key == core::BlockOptionPrioritizeOrder) {
        option = SS_OPTION_PLACE_ASAP;
        return true;
    }

    bfError << "Unrecognized block option.";
    return false;
}

// PARAMETERS METHODS
// ==================

bool SimulinkBlockInformation::getStringParameterAtIndex(const ParameterIndex idx,
                                                         std::string& value) const
{
    const mxArray* blockParam = ssGetSFcnParam(pImpl->simstruct, idx);
    return MxAnyType(blockParam).asString(value);
}

bool SimulinkBlockInformation::getScalarParameterAtIndex(const ParameterIndex idx,
                                                         double& value) const
{
    const mxArray* blockParam = ssGetSFcnParam(pImpl->simstruct, idx);
    return MxAnyType(blockParam).asDouble(value);
}

bool SimulinkBlockInformation::getBooleanParameterAtIndex(const ParameterIndex idx,
                                                          bool& value) const
{
    double tmpValue = 0;
    const mxArray* blockParam = ssGetSFcnParam(pImpl->simstruct, idx);

    // The Simulink mask often doesn't store boolean data from the mask as bool but as double.
    // Calling asBool() will fail in this case. If this happens, asDouble() is used as fallback.
    if (MxAnyType(blockParam).asBool(value)) {
        return true;
    }
    else if (MxAnyType(blockParam).asDouble(tmpValue)) {
        value = static_cast<bool>(tmpValue);
        return true;
    }

    bfError << "Failed to parse bool parameter";
    return false;
}

bool SimulinkBlockInformation::getCellAtIndex(const ParameterIndex idx, AnyCell& value) const
{
    const mxArray* blockParam = ssGetSFcnParam(pImpl->simstruct, idx);
    return MxAnyType(blockParam).asAnyCell(value);
}

bool SimulinkBlockInformation::getStructAtIndex(const ParameterIndex idx, AnyStruct& value) const
{
    const mxArray* blockParam = ssGetSFcnParam(pImpl->simstruct, idx);
    return MxAnyType(blockParam).asAnyStruct(value);
}

bool SimulinkBlockInformation::getVectorAtIndex(const ParameterIndex idx,
                                                std::vector<double>& value) const
{
    const mxArray* blockParam = ssGetSFcnParam(pImpl->simstruct, idx);
    return MxAnyType(blockParam).asVectorDouble(value);
}

bool SimulinkBlockInformation::getStringFieldAtIndex(const ParameterIndex idx,
                                                     const std::string& fieldName,
                                                     std::string& value) const
{
    AnyStruct s;

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

bool SimulinkBlockInformation::getScalarFieldAtIndex(const ParameterIndex idx,
                                                     const std::string& fieldName,
                                                     double& value) const
{
    AnyStruct s;

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

bool SimulinkBlockInformation::getBooleanFieldAtIndex(const ParameterIndex idx,
                                                      const std::string& fieldName,
                                                      bool& value) const
{
    AnyStruct s;

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

bool SimulinkBlockInformation::getCellFieldAtIndex(const ParameterIndex idx,
                                                   const std::string& fieldName,
                                                   AnyCell& value) const
{
    AnyStruct s;

    if (!getStructAtIndex(idx, s)) {
        bfError << "Failed to get struct at index " << idx << ".";
        return false;
    }

    if (s.find(fieldName) == s.end()) {
        bfError << "Struct at index " << idx << " does not contain any " << fieldName << " field.";
        return false;
    }

    return s.at(fieldName)->asAnyCell(value);
}

bool SimulinkBlockInformation::getVectorDoubleFieldAtIndex(const ParameterIndex idx,
                                                           const std::string& fieldName,
                                                           std::vector<double>& value) const
{
    AnyStruct s;

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

// PORT INFORMATION SETTERS
// ========================

bool SimulinkBlockInformation::setNumberOfInputPorts(const unsigned numberOfPorts)
{
    return ssSetNumInputPorts(pImpl->simstruct, numberOfPorts);
}

bool SimulinkBlockInformation::setNumberOfOutputPorts(const unsigned numberOfPorts)
{
    return ssSetNumOutputPorts(pImpl->simstruct, numberOfPorts);
}

bool SimulinkBlockInformation::setInputPortVectorSize(const PortIndex idx, const VectorSize& size)
{
    if (size == core::Signal::DynamicSize) {
        // TODO: in this case, explore how to use mdlSetOutputPortDimensionInfo and
        // mdlSetDefaultPortDimensionInfo
        return ssSetInputPortVectorDimension(pImpl->simstruct, idx, DYNAMICALLY_SIZED);
    }

    return ssSetInputPortVectorDimension(pImpl->simstruct, idx, size);
}

bool SimulinkBlockInformation::setInputPortMatrixSize(const PortIndex idx, const MatrixSize& size)
{
    // Refer to: https://it.mathworks.com/help/simulink/sfg/sssetoutputportmatrixdimensions.html
    if (size.first == core::Signal::DynamicSize || size.second == core::Signal::DynamicSize) {
        // TODO: in this case, explore how to use mdlSetOutputPortDimensionInfo and
        // mdlSetDefaultPortDimensionInfo
        ssSetInputPortMatrixDimensions(pImpl->simstruct, idx, DYNAMICALLY_SIZED, DYNAMICALLY_SIZED);
    }

    return ssSetInputPortMatrixDimensions(pImpl->simstruct, idx, size.first, size.first);
}

bool SimulinkBlockInformation::setOutputPortVectorSize(const PortIndex idx, const VectorSize& size)
{
    if (size == core::Signal::DynamicSize) {
        // TODO: in this case, explore how to use mdlSetOutputPortDimensionInfo and
        // mdlSetDefaultPortDimensionInfo
        return ssSetOutputPortVectorDimension(pImpl->simstruct, idx, DYNAMICALLY_SIZED);
    }

    return ssSetOutputPortVectorDimension(pImpl->simstruct, idx, size);
}

bool SimulinkBlockInformation::setOutputPortMatrixSize(const PortIndex idx, const MatrixSize& size)
{
    // Refer to: https://it.mathworks.com/help/simulink/sfg/sssetinputportmatrixdimensions.html
    if (size.first == core::Signal::DynamicSize || size.second == core::Signal::DynamicSize) {
        // TODO: in this case, explore how to use mdlSetOutputPortDimensionInfo and
        // mdlSetDefaultPortDimensionInfo
        return ssSetOutputPortMatrixDimensions(
            pImpl->simstruct, idx, DYNAMICALLY_SIZED, DYNAMICALLY_SIZED);
    }

    return ssSetOutputPortMatrixDimensions(pImpl->simstruct, idx, size.first, size.second);
}

bool SimulinkBlockInformation::setInputPortType(const PortIndex idx, const core::DataType type)
{
    ssSetInputPortDirectFeedThrough(pImpl->simstruct, idx, 1);
    ssSetInputPortDataType(pImpl->simstruct, idx, pImpl->mapPortTypeToSimulink(type));
    return true;
}

bool SimulinkBlockInformation::setOutputPortType(const PortIndex idx, const core::DataType type)
{
    ssSetOutputPortDataType(pImpl->simstruct, idx, pImpl->mapPortTypeToSimulink(type));
    return true;
}

// PORT INFORMATION GETTERS
// ========================

core::BlockInformation::VectorSize
SimulinkBlockInformation::getInputPortWidth(const PortIndex idx) const
{
    return ssGetInputPortWidth(pImpl->simstruct, idx);
}

core::BlockInformation::VectorSize
SimulinkBlockInformation::getOutputPortWidth(const PortIndex idx) const
{
    return ssGetOutputPortWidth(pImpl->simstruct, idx);
}

core::InputSignalPtr SimulinkBlockInformation::getInputPortSignal(const PortIndex idx,
                                                                  const VectorSize size) const
{
    // Read if the signal is contiguous or non-contiguous
    boolean_T isContiguous = ssGetInputPortRequiredContiguous(pImpl->simstruct, idx);
    core::Signal::DataFormat sigDataFormat = isContiguous
                                                 ? core::Signal::DataFormat::CONTIGUOUS_ZEROCOPY
                                                 : core::Signal::DataFormat::NONCONTIGUOUS;

    // Check if the signal is dynamically sized (which means that the dimension
    // cannot be read)
    bool isDynamicallySized = (ssGetInputPortWidth(pImpl->simstruct, idx) == DYNAMICALLY_SIZED);

    // Note that if the signal is dynamically sized, portWidth is necessary
    if (isDynamicallySized && size == core::Signal::DynamicSize) {
        bfError << "Trying to get a dynamically sized signal without specifying its size.";
        return {};
    }

    // Read the width of the signal if it is not provided as input and the signal is not
    // dynamically sized
    VectorSize signalSize = size;
    if (!isDynamicallySized && size == core::Signal::DynamicSize) {
        signalSize = ssGetInputPortWidth(pImpl->simstruct, idx);
    }

    // Get the data type of the Signal if set (default: double)
    DTypeId dataType = ssGetInputPortDataType(pImpl->simstruct, idx);

    switch (sigDataFormat) {
        case core::Signal::DataFormat::CONTIGUOUS_ZEROCOPY: {
            // Initialize the signal
            auto signal =
                std::make_shared<core::Signal>(core::Signal::DataFormat::CONTIGUOUS_ZEROCOPY,
                                               pImpl->mapSimulinkToPortType(dataType));
            signal->setWidth(signalSize);

            // Initialize signal's data
            if (!signal->initializeBufferFromContiguousZeroCopy(
                    ssGetInputPortSignal(pImpl->simstruct, idx))) {
                bfError << "Failed to inititialize CONTIGUOUS_ZEROCOPY signal at index " << idx
                        << ".";
                return {};
            }

            // Check signal validity
            if (!signal->isValid()) {
                bfError << "Input signal at index " << idx << " is not valid.";
                return {};
            }

            return signal;
        }
        case core::Signal::DataFormat::NONCONTIGUOUS: {
            // Initialize the signal
            auto signal = std::make_shared<core::Signal>(core::Signal::DataFormat::NONCONTIGUOUS,
                                                         pImpl->mapSimulinkToPortType(dataType));
            signal->setWidth(signalSize);

            // Initialize signal's data
            InputPtrsType port = ssGetInputPortSignalPtrs(pImpl->simstruct, idx);
            if (!signal->initializeBufferFromNonContiguous(static_cast<const void* const*>(port))) {
                bfError << "Failed to inititialize NONCONTIGUOUS signal at index " << idx << ".";
                return {};
            }

            // Check signal validity
            if (!signal->isValid()) {
                bfError << "Input signal at index " << idx << " is not valid.";
                return {};
            }

            return signal;
        }
        case core::Signal::DataFormat::CONTIGUOUS: {
            bfError << "Failed to inititialize CONTIGUOUS signal at index " << idx << "."
                    << std::endl
                    << "CONTIGUOUS input signals are not yet supported. "
                    << "Use CONTIGUOUS_ZEROCOPY instead.";
            return {};
        }
    }
}

core::OutputSignalPtr SimulinkBlockInformation::getOutputPortSignal(const PortIndex idx,
                                                                    const VectorSize size) const
{
    // Check if the signal is dynamically sized (which means that the dimension
    // cannot be read)
    bool isDynamicallySized = (ssGetOutputPortWidth(pImpl->simstruct, idx) == DYNAMICALLY_SIZED);

    // Note that if the signal is dynamically sized, portWidth is necessary
    if (isDynamicallySized && size == core::Signal::DynamicSize) {
        bfError << "Trying to get a dynamically sized signal without specifying its size.";
        return {};
    }

    // Read the width of the signal if it is not provided as input and the signal is not
    // dynamically sized
    VectorSize signalSize = size;
    if (!isDynamicallySized && size == core::Signal::DynamicSize) {
        signalSize = ssGetOutputPortWidth(pImpl->simstruct, idx);
    }

    // Get the data type of the Signal if set (default: double)
    DTypeId dataType = ssGetOutputPortDataType(pImpl->simstruct, idx);

    auto signal = std::make_shared<core::Signal>(core::Signal::DataFormat::CONTIGUOUS_ZEROCOPY,
                                                 pImpl->mapSimulinkToPortType(dataType));
    signal->setWidth(signalSize);

    if (!signal->initializeBufferFromContiguousZeroCopy(
            ssGetOutputPortSignal(pImpl->simstruct, idx))) {
        bfError << "Failed to inititialize CONTIGUOUS_ZEROCOPY signal at index " << idx << ".";
        return {};
    }

    if (!signal->isValid()) {
        bfError << "Output signal at index " << idx << " is not valid.";
        return {};
    }

    return signal;
}

core::BlockInformation::MatrixSize
SimulinkBlockInformation::getInputPortMatrixSize(const PortIndex idx) const
{
    if (ssGetInputPortNumDimensions(pImpl->simstruct, idx) < 2) {
        bfError << "Signal at index " << idx
                << "does not contain a matrix. Failed to gete its size.";
        return {};
    }

    const int_T* sizes = ssGetInputPortDimensions(pImpl->simstruct, idx);
    return {sizes[0], sizes[1]};
}

core::BlockInformation::MatrixSize
SimulinkBlockInformation::getOutputPortMatrixSize(const PortIndex idx) const
{
    if (ssGetOutputPortNumDimensions(pImpl->simstruct, idx) < 2) {
        bfError << "Signal at index " << idx
                << "does not contain a matrix. Failed to gete its size.";
        return {};
    }

    const int_T* sizes = ssGetOutputPortDimensions(pImpl->simstruct, idx);
    return {sizes[0], sizes[1]};
}

core::DataType SimulinkBlockInformation::Impl::mapSimulinkToPortType(const DTypeId typeId) const
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

DTypeId SimulinkBlockInformation::Impl::mapPortTypeToSimulink(const core::DataType dataType) const
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
}

bool SimulinkBlockInformation::addParameterMetadata(const core::ParameterMetadata& paramMD)
{
    for (auto md : pImpl->m_paramsMetadata) {
        if (md.name == paramMD.name) {
            bfError << "Trying to store an already existing " << md.name << " parameter.";
            return false;
        }
    }

    // Add the new metadata to the block information
    pImpl->m_paramsMetadata.push_back(paramMD);
    return true;
}

bool SimulinkBlockInformation::parseParameters(core::Parameters& parameters)
{
    auto metadataContainsScalarParam = [](const core::ParameterMetadata& md) -> const bool {
        return md.rows == 1 && md.cols == 1;
    };

    for (core::ParameterMetadata paramMD : pImpl->m_paramsMetadata) {

        bool ok;

        // TODO Right now the cells are reshaped to a 1 x NumElements by MxAnyType
        if (paramMD.rows == core::ParameterMetadata::DynamicSize) {
            bfError << "Dynamically sized rows are not currently supported.";
            return false;
        }

        // Handle the case of dynamically sized columns. In this case the metadata passed
        // from the Block (containing DynamicSize) is modified with the length of the
        // vector that is going to be stored.
        // This is necessary in the pipeline for storing the metadata in the RTW file, which should
        // not have any dynamic size.
        const bool hasDynSizeColumns = (paramMD.cols == core::ParameterMetadata::DynamicSize);
        auto handleDynSizeColumns = [](int& sizeToUpdate, const int& realSize) -> const bool {
            if (realSize == core::ParameterMetadata::DynamicSize) {
                bfError << "Trying to store the cols of a dynamically sized parameters, but the "
                        << "metadata does not specify a valid size. Probably the block didn't "
                        << "updat the size in its initialization phase.";
                return false;
            }
            sizeToUpdate = realSize;
            return true;
        };

        switch (paramMD.type) {
            // SCALAR / VECTOR PARAMETERS
            // --------------------------
            //
            // getScalarParameterAtIndex and getVectorAtIndex operate on type double.
            // The cast to other types is handled by storeParameter internally,
            // accordingly to the type stored in the metadata.
            //
            // Despite bool has its own bool parser, considering that both int and double
            // are loaded as double (Simulink limitation), in order to simplify the
            // maintainability of this code, everything is handled as double.
            //
            case core::ParameterType::INT:
            case core::ParameterType::BOOL:
            case core::ParameterType::DOUBLE: {
                if (metadataContainsScalarParam(paramMD)) {
                    double paramValue;
                    if (!getScalarParameterAtIndex(paramMD.index, paramValue)) {
                        bfError << "Failed to get scalar parameter at index " << paramMD.index
                                << ".";
                        return false;
                    }
                    ok = parameters.storeParameter<double>(paramValue, paramMD);
                }
                else {
                    std::vector<double> paramVector;
                    if (!getVectorAtIndex(paramMD.index, paramVector)) {
                        bfError << "Failed to get vector parameter at index " << paramMD.index
                                << ".";
                        return false;
                    }
                    if (hasDynSizeColumns) {
                        if (!handleDynSizeColumns(paramMD.cols, paramVector.size())) {
                            return false;
                        }
                    }
                    ok = parameters.storeParameter<double>(paramVector, paramMD);
                }
                break;
            }
            case core::ParameterType::STRING: {
                if (metadataContainsScalarParam(paramMD)) {
                    std::string paramValue;
                    if (!getStringParameterAtIndex(paramMD.index, paramValue)) {
                        bfError << "Failed to get string parameter at index " << paramMD.index
                                << ".";
                        return false;
                    }
                    ok = parameters.storeParameter<std::string>(paramValue, paramMD);
                }
                else {
                    bfError << "Char arrays are not yet supported.";
                    return false;
                }
                break;
            }
            // CELL PARAMETERS
            // ---------------
            case core::ParameterType::CELL_INT:
            case core::ParameterType::CELL_BOOL:
            case core::ParameterType::CELL_DOUBLE: {
                AnyCell cell;
                if (!getCellAtIndex(paramMD.index, cell)) {
                    bfError << "Failed to get cell parameter at index " << paramMD.index << ".";
                    return false;
                }
                std::vector<double> paramVector;
                for (auto element : cell) {
                    double value;
                    if (!element->asDouble(value)) {
                        bfError << "Failed to parse an element of the cell at index "
                                << paramMD.index << " as a double.";
                        return false;
                    }
                    paramVector.push_back(value);
                }
                if (hasDynSizeColumns) {
                    if (!handleDynSizeColumns(paramMD.cols, paramVector.size())) {
                        return false;
                    }
                }
                ok = parameters.storeParameter<double>(paramVector, paramMD);
                break;
            }
            case core::ParameterType::CELL_STRING: {
                AnyCell cell;
                if (!getCellAtIndex(paramMD.index, cell)) {
                    bfError << "Failed to get cell parameter at index " << paramMD.index << ".";
                    return false;
                }
                std::vector<std::string> paramVector;
                for (auto element : cell) {
                    std::string value;
                    if (!element->asString(value)) {
                        bfError << "Failed to parse an element of the cell at index "
                                << paramMD.index << " as a string.";
                        return false;
                    }
                    paramVector.push_back(value);
                }
                if (hasDynSizeColumns) {
                    if (!handleDynSizeColumns(paramMD.cols, paramVector.size())) {
                        return false;
                    }
                }
                ok = parameters.storeParameter<std::string>(paramVector, paramMD);
                break;
            }
            // STRUCT PARAMETERS
            // -----------------
            case core::ParameterType::STRUCT_INT:
            case core::ParameterType::STRUCT_BOOL:
            case core::ParameterType::STRUCT_DOUBLE: {
                if (metadataContainsScalarParam(paramMD)) {
                    double paramValue;
                    if (!getScalarFieldAtIndex(paramMD.index, paramMD.name, paramValue)) {
                        bfError << "Failed to get scalar field " << paramMD.name
                                << " from the struct at index " << paramMD.index << ".";
                        return false;
                    }
                    ok = parameters.storeParameter<double>(paramValue, paramMD);
                }
                else {
                    std::vector<double> paramVector;
                    if (!getVectorDoubleFieldAtIndex(paramMD.index, paramMD.name, paramVector)) {
                        bfError << "Failed to get vector field " << paramMD.name
                                << " from the struct at index " << paramMD.index << ".";
                        return false;
                    }
                    if (hasDynSizeColumns) {
                        if (!handleDynSizeColumns(paramMD.cols, paramVector.size())) {
                            return false;
                        }
                    }
                    ok = parameters.storeParameter<double>(paramVector, paramMD);
                }
                break;
            }
            case core::ParameterType::STRUCT_STRING: {
                if (metadataContainsScalarParam(paramMD)) {
                    std::string paramValue;
                    if (!getStringFieldAtIndex(paramMD.index, paramMD.name, paramValue)) {
                        bfError << "Failed to get string field " << paramMD.name
                                << " from the struct at index " << paramMD.index << ".";
                        return false;
                    }
                    ok = parameters.storeParameter<std::string>(paramValue, paramMD);
                }
                else {
                    bfError << "Char arrays are not yet supported.";
                    return false;
                }
                break;
            }
            case core::ParameterType::STRUCT_CELL_INT:
            case core::ParameterType::STRUCT_CELL_BOOL:
            case core::ParameterType::STRUCT_CELL_DOUBLE: {
                AnyCell cell;
                std::vector<double> paramVector;
                if (!getCellFieldAtIndex(paramMD.index, paramMD.name, cell)) {
                    bfError << "Failed to get cell field " << paramMD.name
                            << " from the struct at index " << paramMD.index << ".";
                    return false;
                }
                for (auto element : cell) {
                    double value;
                    if (!element->asDouble(value)) {
                        bfError << "Failed to parse an element of the cell field " << paramMD.name
                                << " from the struct at index " << paramMD.index << " as a double.";
                        return false;
                    }
                    paramVector.push_back(value);
                }
                if (hasDynSizeColumns) {
                    if (!handleDynSizeColumns(paramMD.cols, paramVector.size())) {
                        return false;
                    }
                }
                ok = parameters.storeParameter<double>(paramVector, paramMD);
                break;
            }
            case core::ParameterType::STRUCT_CELL_STRING: {
                AnyCell cell;
                std::vector<std::string> paramVector;
                if (!getCellFieldAtIndex(paramMD.index, paramMD.name, cell)) {
                    bfError << "Failed to get cell field " << paramMD.name
                            << " from the struct at index " << paramMD.index << ".";
                    return false;
                }
                for (auto element : cell) {
                    std::string value;
                    if (!element->asString(value)) {
                        bfError << "Failed to parse an element of the cell field " << paramMD.name
                                << " from the struct at index " << paramMD.index << " as a string.";
                        return false;
                    }
                    paramVector.push_back(value);
                }
                if (hasDynSizeColumns) {
                    if (!handleDynSizeColumns(paramMD.cols, paramVector.size())) {
                        return false;
                    }
                }
                ok = parameters.storeParameter<std::string>(paramVector, paramMD);
                break;
            }
        }

        if (!ok) {
            bfError << "Failed to process parameter with index " << paramMD.index << ".";
            return false;
        }
    }

    // Remove the metadata of the parameters already parsed.
    // This is necessary for adding later more metadata and calling again this method
    // (storing again an already stored parameter raises an error).
    pImpl->m_paramsMetadata.clear();

    return true;
}

bool SimulinkBlockInformation::setIOPortsData(const BlockInformation::IOData& ioData)
{
    // Set the number of input ports
    if (!setNumberOfInputPorts(ioData.input.size())) {
        bfError << "Failed to set the number of input ports.";
        return false;
    }

    // Set the number of output ports
    if (!setNumberOfOutputPorts(ioData.output.size())) {
        bfError << "Failed to set the number of output ports.";
        return false;
    }

    // Set the other input ports properties
    for (const auto& portData : ioData.input) {
        if (!updateInputPortData(portData)) {
            return false;
        }
    }

    // Set the other output ports properties
    for (const auto& portData : ioData.output) {
        if (!updateOutputPortData(portData)) {
            return false;
        }
    }

    return true;
}

bool SimulinkBlockInformation::updateInputPortData(const BlockInformation::PortData& portData)
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
            const Rows rows = portDimensions.at(0);
            const Cols cols = portDimensions.at(1);
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

bool SimulinkBlockInformation::updateOutputPortData(const BlockInformation::PortData& portData)
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
            const Rows rows = portDimensions.at(0);
            const Cols cols = portDimensions.at(1);
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

core::BlockInformation::PortData
SimulinkBlockInformation::getInputPortData(const BlockInformation::PortIndex idx) const
{
    const core::DataType dt =
        pImpl->mapSimulinkToPortType(ssGetInputPortDataType(pImpl->simstruct, idx));
    std::vector<int> portDimension;

    switch (ssGetInputPortNumDimensions(pImpl->simstruct, idx)) {
        case 1:
            portDimension = {ssGetInputPortWidth(pImpl->simstruct, idx)};
            break;
        case 2: {
            const auto dims = ssGetInputPortDimensions(pImpl->simstruct, idx);
            portDimension = {dims[0], dims[1]};
            break;
        }
    }

    return std::make_tuple(idx, portDimension, dt);
}

core::BlockInformation::PortData
SimulinkBlockInformation::getOutputPortData(const BlockInformation::PortIndex idx) const
{
    const core::DataType dt =
        pImpl->mapSimulinkToPortType(ssGetOutputPortDataType(pImpl->simstruct, idx));
    std::vector<int> portDimension;

    switch (ssGetOutputPortNumDimensions(pImpl->simstruct, idx)) {
        case 1:
            portDimension = {ssGetOutputPortWidth(pImpl->simstruct, idx)};
            break;
        case 2: {
            const auto dims = ssGetOutputPortDimensions(pImpl->simstruct, idx);
            portDimension = {dims[0], dims[1]};
            break;
        }
    }

    return std::make_tuple(idx, portDimension, dt);
}
