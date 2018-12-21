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
#include "BlockFactory/Mex/Private/SimulinkBlockInformationImpl.h"

#include <memory>
#include <ostream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

using namespace blockfactory;
using namespace blockfactory::mex;

SimulinkBlockInformation::SimulinkBlockInformation(SimStruct* S)
    : pImpl(std::make_unique<impl::SimulinkBlockInformationImpl>(S))
{}

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

    return {};
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

bool SimulinkBlockInformation::addParameterMetadata(const core::ParameterMetadata& paramMD)
{
    for (auto md : pImpl->paramsMetadata) {
        if (md.name == paramMD.name) {
            bfError << "Trying to store an already existing " << md.name << " parameter.";
            return false;
        }
    }

    // Add the new metadata to the block information
    pImpl->paramsMetadata.push_back(paramMD);
    return true;
}

bool SimulinkBlockInformation::parseParameters(core::Parameters& parameters)
{
    auto metadataContainsScalarParam = [](const core::ParameterMetadata& md) -> bool {
        return md.rows == 1 && md.cols == 1;
    };

    for (core::ParameterMetadata paramMD : pImpl->paramsMetadata) {

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
        auto handleDynSizeColumns = [](int& sizeToUpdate, const int& realSize) -> bool {
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
                    if (!pImpl->getScalarParameterAtIndex(paramMD.index, paramValue)) {
                        bfError << "Failed to get scalar parameter at index " << paramMD.index
                                << ".";
                        return false;
                    }
                    ok = parameters.storeParameter<double>(paramValue, paramMD);
                }
                else {
                    std::vector<double> paramVector;
                    if (!pImpl->getVectorAtIndex(paramMD.index, paramVector)) {
                        bfError << "Failed to get vector parameter at index " << paramMD.index
                                << ".";
                        return false;
                    }
                    if (hasDynSizeColumns) {
                        if (!handleDynSizeColumns(paramMD.cols,
                                                  static_cast<int>(paramVector.size()))) {
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
                    if (!pImpl->getStringParameterAtIndex(paramMD.index, paramValue)) {
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
                mxpp::MxCell cell;
                if (!pImpl->getCellAtIndex(paramMD.index, cell)) {
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
                    if (!handleDynSizeColumns(paramMD.cols, static_cast<int>(paramVector.size()))) {
                        return false;
                    }
                }
                ok = parameters.storeParameter<double>(paramVector, paramMD);
                break;
            }
            case core::ParameterType::CELL_STRING: {
                mxpp::MxCell cell;
                if (!pImpl->getCellAtIndex(paramMD.index, cell)) {
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
                    if (!handleDynSizeColumns(paramMD.cols, static_cast<int>(paramVector.size()))) {
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
                    if (!pImpl->getScalarFieldAtIndex(paramMD.index, paramMD.name, paramValue)) {
                        bfError << "Failed to get scalar field " << paramMD.name
                                << " from the struct at index " << paramMD.index << ".";
                        return false;
                    }
                    ok = parameters.storeParameter<double>(paramValue, paramMD);
                }
                else {
                    std::vector<double> paramVector;
                    if (!pImpl->getVectorDoubleFieldAtIndex(
                            paramMD.index, paramMD.name, paramVector)) {
                        bfError << "Failed to get vector field " << paramMD.name
                                << " from the struct at index " << paramMD.index << ".";
                        return false;
                    }
                    if (hasDynSizeColumns) {
                        if (!handleDynSizeColumns(paramMD.cols,
                                                  static_cast<int>(paramVector.size()))) {
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
                    if (!pImpl->getStringFieldAtIndex(paramMD.index, paramMD.name, paramValue)) {
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
                mxpp::MxCell cell;
                std::vector<double> paramVector;
                if (!pImpl->getCellFieldAtIndex(paramMD.index, paramMD.name, cell)) {
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
                    if (!handleDynSizeColumns(paramMD.cols, static_cast<int>(paramVector.size()))) {
                        return false;
                    }
                }
                ok = parameters.storeParameter<double>(paramVector, paramMD);
                break;
            }
            case core::ParameterType::STRUCT_CELL_STRING: {
                mxpp::MxCell cell;
                std::vector<std::string> paramVector;
                if (!pImpl->getCellFieldAtIndex(paramMD.index, paramMD.name, cell)) {
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
                    if (!handleDynSizeColumns(paramMD.cols, static_cast<int>(paramVector.size()))) {
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
    pImpl->paramsMetadata.clear();

    return true;
}

bool SimulinkBlockInformation::setIOPortsData(const BlockInformation::IOData& ioData)
{
    // Set the number of input ports
    if (!pImpl->setNumberOfInputPorts(ioData.input.size())) {
        bfError << "Failed to set the number of input ports.";
        return false;
    }

    // Set the number of output ports
    if (!pImpl->setNumberOfOutputPorts(ioData.output.size())) {
        bfError << "Failed to set the number of output ports.";
        return false;
    }

    // Set the other input ports properties
    for (const auto& portData : ioData.input) {
        if (!pImpl->updateInputPortData(portData)) {
            return false;
        }
    }

    // Set the other output ports properties
    for (const auto& portData : ioData.output) {
        if (!pImpl->updateOutputPortData(portData)) {
            return false;
        }
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
