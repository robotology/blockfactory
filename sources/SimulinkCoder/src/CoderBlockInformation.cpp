/*
 * Copyright (C) 2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * GNU Lesser General Public License v2.1 or any later version.
 */

#include "BlockFactory/SimulinkCoder/CoderBlockInformation.h"
#include "BlockFactory/Core/Log.h"
#include "BlockFactory/Core/Parameter.h"
#include "BlockFactory/Core/Parameters.h"

#include <cassert>
#include <ostream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace blockfactory;
using namespace blockfactory::coder;

struct PortAndSignalData
{
    std::shared_ptr<core::Signal> signal;
    core::Port::Info portInfo;
};

class CoderBlockInformation::impl
{
public:
    std::vector<core::ParameterMetadata> paramsMetadata;

    std::string confBlockName;
    core::Parameters parametersFromRTW;

    using IndexToPortAndSignalDataMap = std::unordered_map<core::Port::Index, PortAndSignalData>;

    IndexToPortAndSignalDataMap inputPortAndSignalMap;
    IndexToPortAndSignalDataMap outputPortAndSignalMap;

    static bool storePortInfo(const core::Port::Info& portInfo,
                              void* signalAddress,
                              IndexToPortAndSignalDataMap& dataMap);

    bool inputPortAtIndexExists(const core::Port::Index idx) const;
    bool outputPortAtIndexExists(const core::Port::Index idx) const;
};

bool CoderBlockInformation::impl::inputPortAtIndexExists(const core::Port::Index idx) const
{
    if (inputPortAndSignalMap.find(idx) == inputPortAndSignalMap.end()) {
        return false;
    }

    return true;
}

bool CoderBlockInformation::impl::outputPortAtIndexExists(const core::Port::Index idx) const
{
    if (outputPortAndSignalMap.find(idx) == outputPortAndSignalMap.end()) {
        return false;
    }

    return true;
}

CoderBlockInformation::CoderBlockInformation()
    : pImpl(std::make_unique<CoderBlockInformation::impl>())
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

bool CoderBlockInformation::setPortsInfo(const core::InputPortsInfo& /*inputPortsInfo*/,
                                         const core::OutputPortsInfo& /*outputPortsInfo*/)
{
    // This method is called only in the Block::configureSizeAndPorts method, which is never called
    // in the Simulink Coder pipeline.
    return false;
}

// PORT INFORMATION GETTERS
// ========================

core::Port::Size::Vector CoderBlockInformation::getInputPortWidth(const core::Port::Index idx) const
{
    if (!pImpl->inputPortAtIndexExists(idx)) {
        bfError << "This block has no input port at index " << idx;
        return 0;
    }

    // mdlRTW writes always a {rows, cols} structure, and vectors are row vectors.
    // This means that their dimension is the cols entry.
    return pImpl->inputPortAndSignalMap.at(idx).portInfo.dimension.at(1);
}

core::Port::Size::Vector
CoderBlockInformation::getOutputPortWidth(const core::Port::Index idx) const
{
    if (!pImpl->outputPortAtIndexExists(idx)) {
        bfError << "This block has no output port at index " << idx;
        return 0;
    }

    // mdlRTW writes always a {rows, cols} structure, and vectors are row vectors.
    // This means that their dimension is the cols entry.
    return pImpl->outputPortAndSignalMap.at(idx).portInfo.dimension.at(1);
}

core::InputSignalPtr CoderBlockInformation::getInputPortSignal(const core::Port::Index idx) const
{
    if (!pImpl->inputPortAtIndexExists(idx)) {
        bfError << "This block has no input port at index " << idx;
        return {};
    }

    // Get the signal
    auto signal = pImpl->inputPortAndSignalMap.at(idx).signal;

    // Check that the port to which the signal is connected does not have dynamic sizes
    for (const auto dim : getInputPortInfo(idx).dimension) {
        if (dim == core::Port::DynamicSize) {
            bfError << "The input port " << idx << " has dynamic sizes. "
                    << "This should not happen in the Simulink Coder implementation.";
            return {};
        }
    }

    if (!signal->isValid()) {
        bfError << "Input signal at index " << idx << " is not valid.";
        return {};
    }

    return std::move(signal);
}

core::OutputSignalPtr CoderBlockInformation::getOutputPortSignal(const core::Port::Index idx) const
{
    if (!pImpl->outputPortAtIndexExists(idx)) {
        bfError << "This block has no output port at index " << idx;
        return {};
    }

    // Get the signal
    auto signal = pImpl->outputPortAndSignalMap.at(idx).signal;

    // Check that the port to which the signal is connected does not have dynamic sizes
    for (const auto dim : getOutputPortInfo(idx).dimension) {
        if (dim == core::Port::DynamicSize) {
            bfError << "The output port " << idx << " has dynamic sizes. "
                    << "This should not happen in the Simulink Coder implementation.";
            return {};
        }
    }

    if (!signal->isValid()) {
        bfError << "Output signal at index " << idx << " is not valid.";
        return {};
    }

    return signal;
}

core::Port::Size::Matrix
CoderBlockInformation::getInputPortMatrixSize(const core::Port::Index idx) const
{
    if (!pImpl->inputPortAtIndexExists(idx)) {
        bfError << "This block has no input port at index " << idx;
        return {};
    }

    auto dims = pImpl->inputPortAndSignalMap.at(idx).portInfo.dimension;

    assert(dims.size() >= 2);
    return {dims[0], dims[1]};
}

core::Port::Size::Matrix
CoderBlockInformation::getOutputPortMatrixSize(const core::Port::Index idx) const
{
    if (!pImpl->outputPortAtIndexExists(idx)) {
        bfError << "This block has no output port at index " << idx;
        return {};
    }

    auto dims = pImpl->outputPortAndSignalMap.at(idx).portInfo.dimension;

    assert(dims.size() >= 2);
    return {dims[0], dims[1]};
}

bool CoderBlockInformation::addParameterMetadata(const core::ParameterMetadata& paramMD)
{
    for (const auto& md : pImpl->paramsMetadata) {
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

core::Port::Info CoderBlockInformation::getInputPortInfo(core::Port::Index idx) const
{
    // TODO: this should be ported to an optional object
    if (pImpl->inputPortAndSignalMap.find(idx) == pImpl->inputPortAndSignalMap.end()) {
        bfError << "This block has no input port at index " << idx;
        return {};
    }

    return pImpl->inputPortAndSignalMap.at(idx).portInfo;
}

core::Port::Info CoderBlockInformation::getOutputPortInfo(core::Port::Index idx) const
{
    // TODO: this should be ported to an optional object
    if (pImpl->outputPortAndSignalMap.find(idx) == pImpl->outputPortAndSignalMap.end()) {
        bfError << "This block has no input port at index " << idx;
        return {};
    }

    return pImpl->outputPortAndSignalMap.at(idx).portInfo;
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

bool CoderBlockInformation::impl::storePortInfo(const core::Port::Info& portInfo,
                                                void* signalAddress,
                                                IndexToPortAndSignalDataMap& dataMap)
{
    auto& idx = portInfo.index;
    auto& dataType = portInfo.dataType;
    auto& dimensions = portInfo.dimension;

    if ((dataMap.find(idx) != dataMap.end())) {
        bfError << "This signal was already stored.";
        return false;
    }

    if (!signalAddress) {
        bfError << "The pointer to the signal to store is a nullptr.";
        return false;
    }

    if (dimensions.size() > 2) {
        bfError << "Signals with more than 2 dimensions are not currently supported.";
        return false;
    }

    if (dataType != core::Port::DataType::DOUBLE) {
        bfError << "Only DataType::DOUBLE is currently supported.";
        return false;
    }

    for (const auto dim : dimensions) {
        // Zero-length and dynamically sized ports are not supported here.
        // The functions set{Input,Output}Port() should set concrete port dimensions.
        if (dim <= 0) {
            bfError << "The dimension of the associated port is either equal to zero or set "
                    << "as dynamically sized.";
            return {};
        }
    }

    // Compute the width of the signal
    unsigned numElements = 1;
    for (const auto dim : dimensions) {
        // Compute the overall number of elements. This is needed to configure properly
        // the returned Signal object.
        numElements *= static_cast<unsigned>(dim);
    }

    // Create the signal object
    auto signal =
        std::make_shared<core::Signal>(core::Signal::DataFormat::CONTIGUOUS_ZEROCOPY, dataType);

    // Configure the signal
    if (!signal->initializeBufferFromContiguousZeroCopy(signalAddress, numElements)) {
        bfError << "Failed to configure buffer for signal connected to the port with index " << idx
                << ".";
        return false;
    }

    // Store the signal and the port data
    dataMap.insert({idx, {signal, portInfo}});

    return true;
}

bool CoderBlockInformation::setInputPort(const core::Port::Info& portInfo, void* signalAddress)
{
    if (!pImpl->storePortInfo(portInfo, signalAddress, pImpl->inputPortAndSignalMap)) {
        bfError << "Failed to store data of the input signal plugged at port with index "
                << portInfo.index;
        return false;
    }

    return true;
}

bool CoderBlockInformation::setOutputPort(const core::Port::Info& portInfo, void* signalAddress)
{
    if (!pImpl->storePortInfo(portInfo, signalAddress, pImpl->outputPortAndSignalMap)) {
        bfError << "Failed to store data of the output signal plugged at port with index "
                << portInfo.index;
        return false;
    }

    return true;
}
