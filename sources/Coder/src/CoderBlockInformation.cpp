/*
 * Copyright (C) 2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * GNU Lesser General Public License v2.1 or any later version.
 */

#include "BlockFactory/Coder/CoderBlockInformation.h"
#include "BlockFactory/Core/Log.h"
#include "BlockFactory/Core/Parameter.h"
#include "BlockFactory/Core/Parameters.h"

#include <ostream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace blockfactory;
using namespace blockfactory::coder;

class CoderBlockInformation::impl
{
public:
    unsigned numberOfInputs;
    unsigned numberOfOutputs;

    std::vector<core::ParameterMetadata> paramsMetadata;
    std::unordered_map<BlockInformation::PortIndex, std::shared_ptr<core::Signal>> inputSignals;
    std::unordered_map<BlockInformation::PortIndex, std::shared_ptr<core::Signal>> outputSignals;

    std::string confBlockName;
    core::Parameters parametersFromRTW;

    std::unordered_map<BlockInformation::PortIndex, BlockInformation::PortDimension>
        inputPortDimensions;
    std::unordered_map<BlockInformation::PortIndex, BlockInformation::PortDimension>
        outputPortDimensions;
};

CoderBlockInformation::CoderBlockInformation()
    : pImpl{new CoderBlockInformation::impl()}
{}

CoderBlockInformation::~CoderBlockInformation() = default;

// BLOCK OPTIONS METHODS
// =====================

bool CoderBlockInformation::optionFromKey(const std::string& /*key*/, double& /*option*/) const
{
    return true;
}

// PORT INFORMATION SETTERS
// ========================

bool CoderBlockInformation::setIOPortsData(const BlockInformation::IOData& /*ioData*/)
{
    // This method is called only in the Block::configureSizeAndPorts method, which is never called
    // in the Simulink Coder pipeline.
    return false;
}

// PORT INFORMATION GETTERS
// ========================

core::BlockInformation::VectorSize
CoderBlockInformation::getInputPortWidth(const PortIndex idx) const
{
    if (pImpl->inputPortDimensions.find(idx) == pImpl->inputPortDimensions.end()) {
        bfError << "Failed to get width of signal at index " << idx << ".";
        return 0;
    }

    // mdlRTW writes always a {rows, cols} structure, and vectors are row vectors.
    // This means that their dimension is the cols entry.
    return pImpl->inputPortDimensions.at(idx).at(1);
}

core::BlockInformation::VectorSize
CoderBlockInformation::getOutputPortWidth(const PortIndex idx) const
{
    if (pImpl->outputPortDimensions.find(idx) == pImpl->outputPortDimensions.end()) {
        bfError << "Failed to get width of signal at index " << idx << ".";
        return 0;
    }

    // mdlRTW writes always a {rows, cols} structure, and vectors are row vectors.
    // This means that their dimension is the cols entry.
    return pImpl->outputPortDimensions.at(idx).at(1);
}

core::InputSignalPtr CoderBlockInformation::getInputPortSignal(const PortIndex idx,
                                                               const VectorSize size) const
{
    if (pImpl->inputSignals.find(idx) == pImpl->inputSignals.end()) {
        bfError << "Trying to get non-existing signal " << idx << ".";
        return {};
    }

    // TODO: portWidth is used only if the signal is dynamically sized. In Simulink, in this case
    // the size is gathered from the SimStruct. From the coder instead? Is it possible having
    // a signal with dynamic size in the rtw file??
    // TODO: is it better this check or the one implemented in getOutputPortSignal?
    if (size != core::Signal::DynamicSize && pImpl->inputSignals.at(idx)->getWidth() != size) {
        bfError << "Signals with dynamic sizes (index " << idx
                << ") are not supported by the CoderBlockInformation.";
        return {};
    }

    if (!pImpl->inputSignals.at(idx)->isValid()) {
        bfError << "Input signal at index " << idx << " is not valid.";
        return {};
    }

    return pImpl->inputSignals.at(idx);
}

core::OutputSignalPtr CoderBlockInformation::getOutputPortSignal(const PortIndex idx,
                                                                 const VectorSize /*size*/) const
{
    if (pImpl->outputSignals.find(idx) == pImpl->outputSignals.end()) {
        bfError << "Trying to get non-existing signal " << idx << ".";
        return {};
    }

    if (pImpl->outputSignals.at(idx)->getWidth() == core::Signal::DynamicSize) {
        bfError << "Signals with dynamic sizes (index " << idx
                << ") are not supported by the CoderBlockInformation.";
        return {};
    }

    if (!pImpl->outputSignals.at(idx)->isValid()) {
        bfError << "Output signal at index " << idx << " is not valid.";
        return {};
    }

    return pImpl->outputSignals.at(idx);
}

core::BlockInformation::MatrixSize
CoderBlockInformation::getInputPortMatrixSize(const BlockInformation::PortIndex idx) const
{
    if (pImpl->inputPortDimensions.find(idx) == pImpl->inputPortDimensions.end()) {
        bfError << "Trying to get the size of non-existing signal " << idx << ".";
        return {};
    }

    return {pImpl->inputPortDimensions.at(idx)[0], pImpl->inputPortDimensions.at(idx)[1]};
}

core::BlockInformation::MatrixSize
CoderBlockInformation::getOutputPortMatrixSize(const BlockInformation::PortIndex idx) const
{
    if (pImpl->outputPortDimensions.find(idx) == pImpl->outputPortDimensions.end()) {
        bfError << "Trying to get the size of non-existing signal " << idx << ".";
        return {};
    }

    return {pImpl->outputPortDimensions.at(idx)[0], pImpl->outputPortDimensions.at(idx)[1]};
}

bool CoderBlockInformation::addParameterMetadata(const core::ParameterMetadata& paramMD)
{
    for (auto md : pImpl->paramsMetadata) {
        if (md.name == paramMD.name) {
            bfError << "Trying to store an already existing " << md.name << " parameter.";
            return false;
        }
    }

    pImpl->paramsMetadata.push_back(paramMD);
    return true;
}

// PARAMETERS METHODS
// ==================

bool CoderBlockInformation::parseParameters(core::Parameters& parameters)
{
    if (pImpl->parametersFromRTW.getNumberOfParameters() == 0) {
        bfError << "The Parameters object containing the parameters to parse is empty.";
        return false;
    }

    for (core::ParameterMetadata& md : pImpl->paramsMetadata) {
        // Check that all the parameters that are parsed have already been stored from the coder
        if (!pImpl->parametersFromRTW.existName(md.name)) {
            bfError << "Trying to get a parameter value for " << md.name
                    << ", but its value has never been stored.";
            return false;
        }

        // Handle the case of dynamically sized columns. In this case the metadata passed
        // from the Block (containing DynamicSize) is modified with the length of the
        // vector that is going to be stored.
        if (md.cols == core::ParameterMetadata::DynamicSize) {
            const auto colsFromRTW = pImpl->parametersFromRTW.getParameterMetadata(md.name).cols;
            if (colsFromRTW == core::ParameterMetadata::DynamicSize) {
                bfError << "Trying to store the cols of a dynamically sized parameters, but the "
                        << "metadata does not specify a valid size. Probably the block didn't "
                        << "updat the size in its initialization phase.";
                return false;
            }
            md.cols = colsFromRTW;
        }

        if (md != pImpl->parametersFromRTW.getParameterMetadata(md.name)) {
            bfError << "Trying to parse a parameter which metadata differs from the metadata "
                    << "stored by Simulink Coder.";
            return false;
        }
    }

    // This implementation of BlockInformation contains all the parameters from the very beginning,
    // stored using the storeRTWParameters method. Here for simplicity all the stored parameters are
    // returned, even if the metadata contain only a subset of them.
    parameters = pImpl->parametersFromRTW;
    return true;
}

core::BlockInformation::PortData
CoderBlockInformation::getInputPortData(BlockInformation::PortIndex idx) const
{
    // TODO: hardcoded DataType::DOUBLE.
    return std::make_tuple(idx, pImpl->inputPortDimensions.at(idx), core::DataType::DOUBLE);
}

core::BlockInformation::PortData
CoderBlockInformation::getOutputPortData(BlockInformation::PortIndex idx) const
{
    // TODO: hardcoded DataType::DOUBLE.
    return std::make_tuple(idx, pImpl->outputPortDimensions.at(idx), core::DataType::DOUBLE);
}

bool CoderBlockInformation::storeRTWParameters(const core::Parameters& parameters)
{
    if (parameters.getNumberOfParameters() == 0) {
        bfError << "The Parameters object passed doesn't contain any parameter.";
        return false;
    }

    pImpl->parametersFromRTW = parameters;
    return true;
}

bool CoderBlockInformation::setInputSignal(const PortIndex portNumber,
                                           void* address,
                                           const PortDimension& dims)
{
    if ((pImpl->inputSignals.find(portNumber) != pImpl->inputSignals.end())
        || (pImpl->inputPortDimensions.find(portNumber) != pImpl->inputPortDimensions.end())) {
        bfError << "The signal " << portNumber << "has already been previously stored.";
        return false;
    }

    if (!address) {
        bfError << "The pointer to the signal to store is a nullptr.";
        return false;
    }

    if (dims.size() > 2) {
        bfError << "Signal with more than 2 dimensions are not currently supported.";
        return false;
    }

    // Store the input signal
    // TODO: hardcoded DataType::DOUBLE
    pImpl->inputSignals.insert(
        {portNumber,
         std::make_shared<core::Signal>(core::Signal::DataFormat::CONTIGUOUS_ZEROCOPY,
                                        core::DataType::DOUBLE)});

    // Compute the width of the signal
    unsigned numElements = 1;
    for (auto dimension : dims) {
        numElements *= dimension;
    }

    // Configure the signal
    pImpl->inputSignals[portNumber]->setWidth(numElements);
    if (!pImpl->inputSignals[portNumber]->initializeBufferFromContiguousZeroCopy(address)) {
        bfError << "Failed to configure buffer for input signal connected to the port with index "
                << portNumber << ".";
        return false;
    }

    // Store the dimensions in the map
    pImpl->inputPortDimensions.emplace(portNumber, dims);

    return true;
}

bool CoderBlockInformation::setOutputSignal(const PortIndex portNumber,
                                            void* address,
                                            const PortDimension& dims)
{
    if ((pImpl->outputSignals.find(portNumber) != pImpl->outputSignals.end())
        || (pImpl->outputPortDimensions.find(portNumber) != pImpl->outputPortDimensions.end())) {
        bfError << "The signal " << portNumber << "has already been previously stored.";
        return false;
    }

    if (!address) {
        bfError << "The pointer to the signal to store is a nullptr.";
        return false;
    }

    if (dims.size() > 2) {
        bfError << "Signal with more than 2 dimensions are not currently supported.";
        return false;
    }

    // Store the output signal
    // TODO: hardcoded DataType::DOUBLE
    pImpl->outputSignals.insert(
        {portNumber,
         std::make_shared<core::Signal>(core::Signal::DataFormat::CONTIGUOUS_ZEROCOPY,
                                        core::DataType::DOUBLE)});

    // Compute the width of the signal
    unsigned numElements = 1;
    for (auto dimension : dims) {
        numElements *= dimension;
    }

    // Configure the signal
    pImpl->outputSignals[portNumber]->setWidth(numElements);
    if (!pImpl->outputSignals[portNumber]->initializeBufferFromContiguousZeroCopy(address)) {
        bfError << "Failed to configure buffer for output signal connected to the port with index "
                << portNumber << ".";
        return false;
    }

    // Store the dimensions in the map
    pImpl->outputPortDimensions.emplace(portNumber, dims);

    return true;
}
