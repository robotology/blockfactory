/*
 * Copyright (C) 2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * GNU Lesser General Public License v2.1 or any later version.
 */

#include "BlockFactory/Simulink/SimulinkBlockInformation.h"
#include "BlockFactory/Core/Log.h"
#include "BlockFactory/Core/Parameter.h"
#include "BlockFactory/Core/Parameters.h"
#include "BlockFactory/Core/Signal.h"
#include "BlockFactory/Simulink/Private/SimulinkBlockInformationImpl.h"

#include <cassert>
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
    return pImpl->optionFromKey(key, option);
}

// PORT INFORMATION GETTERS
// ========================

core::Port::Size::Vector
SimulinkBlockInformation::getInputPortWidth(const core::Port::Index idx) const
{
    core::Port::Info portInfo = getInputPortInfo(idx);

    if (portInfo.dimension.size() != 1) {
        bfError << "Input port at index " << idx
                << "does not contain a vector. Failed to get its size.";
        assert(portInfo.dimension.size() != 1);
        return {};
    }

    return portInfo.dimension[0];
}

core::Port::Size::Vector
SimulinkBlockInformation::getOutputPortWidth(const core::Port::Index idx) const
{
    core::Port::Info portInfo = getOutputPortInfo(idx);

    if (portInfo.dimension.size() != 1) {
        bfError << "Output port at index " << idx
                << "does not contain a vector. Failed to get its size.";
        assert(portInfo.dimension.size() != 1);
        return {};
    }

    return portInfo.dimension[0];
}

core::InputSignalPtr SimulinkBlockInformation::getInputPortSignal(const core::Port::Index idx) const
{
    // Get the PortData
    core::Port::Info portInfo = getInputPortInfo(idx);

    // This can happen only if the Block attempts to get the Signal during the
    // Block::configureSizeAndPorts step. This has undefined behavior, and it should not
    // be allowed.
    if (pImpl->isInputPortDynamicallySized(idx)) {
        bfError << "The input port " << idx
                << " has dynamic sizes. Probably the engine hasn't propagated them "
                << "and the attached signal is not yet available.";
        return {};
    }

    // Read if the signal is contiguous or non-contiguous
    boolean_T isContiguous = pImpl->isInputSignalAtIdxContiguous(idx);
    auto signalDataFormat = isContiguous ? core::Signal::DataFormat::CONTIGUOUS_ZEROCOPY
                                         : core::Signal::DataFormat::NONCONTIGUOUS;

    // Read the number of expected elements. This will match the buffer size of
    // the associated Signal object.
    size_t nrOfElements = pImpl->getNrOfInputPortElements(idx);

    switch (signalDataFormat) {
        case core::Signal::DataFormat::CONTIGUOUS_ZEROCOPY: {
            // Get the buffer pointer from Simulink
            auto signalRawPtr = pImpl->getContiguousSignalRawPtrFromInputPort(idx);
            if (!signalRawPtr) {
                bfError << "Failed to get input signal at index " << idx << ".";
            }

            // Initialize the signal
            auto signal = std::make_shared<core::Signal>(
                core::Signal::DataFormat::CONTIGUOUS_ZEROCOPY, portInfo.dataType);

            // Initialize signal's data
            if (!signal->initializeBufferFromContiguousZeroCopy(signalRawPtr, nrOfElements)) {
                bfError << "Failed to initialize CONTIGUOUS_ZEROCOPY signal connected to "
                        << "input port at index " << idx << ".";
                return {};
            }

            // Check signal validity
            if (!signal->isValid()) {
                bfError << "Input signal at index " << idx << " is not valid.";
                return {};
            }

            return std::move(signal);
        }
        case core::Signal::DataFormat::NONCONTIGUOUS: {
            // Get the buffer pointer from Simulink
            auto signalRawPtr = pImpl->getNonContiguousSignalRawPtrFromInputPort(idx);
            if (!signalRawPtr) {
                bfError << "Failed to get input signal at index " << idx << ".";
            }

            // Initialize the signal
            auto signal = std::make_shared<core::Signal>(core::Signal::DataFormat::NONCONTIGUOUS,
                                                         portInfo.dataType);

            // Initialize signal's data
            if (!signal->initializeBufferFromNonContiguous(signalRawPtr, nrOfElements)) {
                bfError << "Failed to initialize NONCONTIGUOUS signal connected to "
                        << "input port at index " << idx << ".";
                return {};
            }

            // Check signal validity
            if (!signal->isValid()) {
                bfError << "Input signal at index " << idx << " is not valid.";
                return {};
            }

            return std::move(signal);
        }
        case core::Signal::DataFormat::CONTIGUOUS: {
            bfError << "Failed to inititialize CONTIGUOUS signal at index " << idx << "."
                    << std::endl
                    << "CONTIGUOUS input signals are not yet supported. "
                    << "Use CONTIGUOUS_ZEROCOPY instead, they are more efficient.";
            return {};
        }
    }

    return {};
}

core::OutputSignalPtr
SimulinkBlockInformation::getOutputPortSignal(const core::Port::Index idx) const
{
    // Get the PortData
    core::Port::Info portInfo = getOutputPortInfo(idx);

    // This can happen only if the Block attempts to get the Signal during the
    // Block::configureSizeAndPorts step. This has undefined behavior, and it should not
    // be allowed.
    if (pImpl->isOutputPortDynamicallySized(idx)) {
        bfError << "The output port " << idx
                << " has dynamic sizes. Probably the engine hasn't propagated them "
                << "and the attached signal is not yet available.";
        return {};
    }

    // Read the number of expected elements. This will match the buffer size of
    // the associated Signal object.
    size_t nrOfElements = pImpl->getNrOfOutputPortElements(idx);

    // Get the buffer pointer from Simulink
    auto signalRawPtr = pImpl->getSignalRawPtrFromOutputPort(idx);
    if (!signalRawPtr) {
        bfError << "Failed to get output signal at index " << idx << ".";
    }

    // Initialize the signal
    auto signal = std::make_shared<core::Signal>(core::Signal::DataFormat::CONTIGUOUS_ZEROCOPY,
                                                 portInfo.dataType);

    // Initialize signal's data
    if (!signal->initializeBufferFromContiguousZeroCopy(signalRawPtr, nrOfElements)) {
        bfError << "Failed to initialize CONTIGUOUS_ZEROCOPY signal connected to "
                << "output port at index " << idx << ".";
        return {};
    }

    // Check signal validity
    if (!signal->isValid()) {
        bfError << "Input signal at index " << idx << " is not valid.";
        return {};
    }

    return signal;
}

core::Port::Size::Matrix
SimulinkBlockInformation::getInputPortMatrixSize(const core::Port::Index idx) const
{
    core::Port::Info portInfo = getInputPortInfo(idx);

    if (portInfo.dimension.size() != 2) {
        bfError << "Input port at index " << idx
                << "does not contain a matrix. Failed to get its size.";
        assert(portInfo.dimension.size() != 2);
        return {};
    }

    return {portInfo.dimension[0], portInfo.dimension[1]};
}

core::Port::Size::Matrix
SimulinkBlockInformation::getOutputPortMatrixSize(const core::Port::Index idx) const
{
    core::Port::Info portInfo = getOutputPortInfo(idx);

    if (portInfo.dimension.size() != 2) {
        bfError << "Output port at index " << idx
                << "does not contain a matrix. Failed to get its size.";
        assert(portInfo.dimension.size() != 2);
        return {};
    }

    return {portInfo.dimension[0], portInfo.dimension[1]};
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

bool SimulinkBlockInformation::setPortsInfo(const core::InputPortsInfo& inputPortsInfo,
                                            const core::OutputPortsInfo& outputPortsInfo)
{
    // Set the number of input ports
    if (!pImpl->setNumberOfInputPorts(inputPortsInfo.size())) {
        bfError << "Failed to set the number of input ports.";
        return false;
    }

    // Set the number of output ports
    if (!pImpl->setNumberOfOutputPorts(outputPortsInfo.size())) {
        bfError << "Failed to set the number of output ports.";
        return false;
    }

    // Set the other input ports properties
    for (const auto& portInfo : inputPortsInfo) {
        if (!pImpl->updateInputPortInfo(portInfo)) {
            return false;
        }
    }

    // Set the other output ports properties
    for (const auto& portInfo : outputPortsInfo) {
        if (!pImpl->updateOutputPortInfo(portInfo)) {
            return false;
        }
    }

    return true;
}

core::Port::Info SimulinkBlockInformation::getInputPortInfo(const core::Port::Index idx) const
{
    return pImpl->getInputPortInfo(idx);
}

core::Port::Info SimulinkBlockInformation::getOutputPortInfo(const core::Port::Index idx) const
{
    return pImpl->getOutputPortInfo(idx);
}
