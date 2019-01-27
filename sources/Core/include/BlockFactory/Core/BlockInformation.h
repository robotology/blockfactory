/*
 * Copyright (C) 2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * GNU Lesser General Public License v2.1 or any later version.
 */

#ifndef BLOCKFACTORY_CORE_BLOCKINFORMATION_H
#define BLOCKFACTORY_CORE_BLOCKINFORMATION_H

#include <memory>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace blockfactory {
    namespace core {
        class BlockInformation;
        class ParameterMetadata;
        class Parameters;
        class Signal;
        using InputSignalPtr = std::shared_ptr<const blockfactory::core::Signal>;
        using OutputSignalPtr = std::shared_ptr<blockfactory::core::Signal>;
        enum class DataType;
        // List of possible key for defining block options:
        extern const std::string BlockOptionPrioritizeOrder;
    } // namespace core
} // namespace blockfactory

/**
 * @brief Abstract class for storing generic core::Block properties
 *
 * core::BlockInformation provides an interface for handling implementation-specific properties such
 * as input / output number, size and type, number of parameters, ...
 *
 * A core::Block needs to know on what kind of data it operates, and retrieving this information is
 * often specific on the framework on top of which blocks run. In order to allow using the same
 * core::Block class from different frameworks (e.g. Simulink, C++, etc), different implementation
 * of this interface can be developed to provide a transparent translation of such functionalities.
 *
 * As an example, take the core::BlockInformation::parseParameters. In Simulink, parameters are read
 * from the block masks and Matlab provides APIs for reading them. The mex::SimulinkBlockInformation
 * implementation will use those APIs. However, if you want to call the same block class (which is
 * just a wrapper of an algorithm) from a pure C++ main, parameters are read e.g. from an xml file.
 * In this case, core::BlockInformation::parseParameters will parse the xml and fill the
 * core::Parameters argument.
 *
 * @see core::Block, core::Parameters, core::Signal
 */
class blockfactory::core::BlockInformation
{
public:
    using Rows = int;
    using Cols = int;
    using PortIndex = int;

    using VectorSize = int;
    using MatrixSize = std::pair<Rows, Cols>;

    struct Port
    { // The struct provides an enum scope
        enum
        {
            Index = 0,
            Dimensions = 1,
            DataType = 2,
        };
    };
    using PortDimension = std::vector<int>;
    using PortData = std::tuple<PortIndex, PortDimension, blockfactory::core::DataType>;

    BlockInformation() = default;
    virtual ~BlockInformation() = default;

    // =====================
    // BLOCK OPTIONS METHODS
    // =====================

    /**
     * @brief Convert a block option from its string identifier to a specific implementation
     *
     * @param[in]  key Identifier of the block option.
     * @param[out] option Implementation-specific block option.
     * @return True if the option has been converted, false otherwise.
     */
    virtual bool optionFromKey(const std::string& key, double& option) const = 0;

    // ==================
    // PARAMETERS METHODS
    // ==================

    /**
     * @brief Parse the core::Block parameters
     *
     * This method allows defining how to gather block parameters from a specific implementation.
     *
     * @param[out] parameters A container filled with the parsed parameters.
     * @return True for success, false otherwise.
     */
    virtual bool parseParameters(blockfactory::core::Parameters& parameters) = 0;

    /**
     * @brief Add a parameter metadata
     *
     * In order to gather parameters from the running engine, the block must pass their metadata.
     *
     * @param paramMD The metadata to add.
     * @return True for success, false otherwise.
     *
     * @see core::BlockInformation::parseParameters
     */
    virtual bool addParameterMetadata(const blockfactory::core::ParameterMetadata& paramMD) = 0;

    // ========================
    // PORT INFORMATION SETTERS
    // ========================

    struct IOData;

    /**
     * @brief Set input / output ports data
     *
     * Specify I/O ports data such as core::BlockInformation::PortIndex,
     * core::BlockInformation::PortDimension, and core::DataType storing the information in a
     * core::BlockInformation::IOData structure.
     *
     * @param ioData The structure containing I/O ports data.
     * @return True for success, false otherwise.
     *
     * @note This method should also automatically set the number of inputs and outputs.
     */
    virtual bool setIOPortsData(const IOData& ioData) = 0;

    // ========================
    // PORT INFORMATION GETTERS
    // ========================

    /**
     * @brief Get data of an input port
     *
     * @param idx The index of the port.
     * @return A PortData object containing the port's data.
     */
    virtual PortData getInputPortData(PortIndex idx) const = 0;

    /**
     * @brief Get data of an output port
     *
     * @param idx The index of the port.
     * @return A PortData object containing the port's data.
     */
    virtual PortData getOutputPortData(PortIndex idx) const = 0;

    /**
     * @brief Get the size of a 1D input port
     *
     * @param idx The index of the port.
     * @return The size of the port.
     */
    virtual VectorSize getInputPortWidth(const PortIndex idx) const = 0;

    /**
     * @brief Get the size of a 1D output port
     *
     * @param idx The index of the port.
     * @return The size of the port.
     */
    virtual VectorSize getOutputPortWidth(const PortIndex idx) const = 0;

    /**
     * @brief Get the size of a 2D input port
     *
     * @param idx The index of the port.
     * @return The size of the port.
     */
    virtual MatrixSize getInputPortMatrixSize(const PortIndex idx) const = 0;

    /**
     * @brief Get the size of a 2D output port
     *
     * @param idx The index of the port.
     * @return The size of the port.
     */
    virtual MatrixSize getOutputPortMatrixSize(const PortIndex idx) const = 0;

    // =============
    // BLOCK SIGNALS
    // =============

    /**
     * @brief Get the signal connected to a 1D input port
     *
     * @param idx The index of the port.
     * @param size The size of the signal.
     * @return The pointer to the signal connected to the input port for success, a `nullptr`
     *         otherwise.
     */
    virtual blockfactory::core::InputSignalPtr
    getInputPortSignal(const PortIndex idx, const VectorSize size = -1) const = 0;

    /**
     * @brief Get the signal connected to a 1D output port
     *
     * @param idx The index of the port.
     * @param size The size of the signal.
     *@return The pointer to the signal connected to the output port for success, a `nullptr`
     *         otherwise.
     */
    virtual blockfactory::core::OutputSignalPtr
    getOutputPortSignal(const PortIndex idx, const VectorSize size = -1) const = 0;
};

struct blockfactory::core::BlockInformation::IOData
{
    std::vector<BlockInformation::PortData> input;
    std::vector<BlockInformation::PortData> output;
};

#endif // BLOCKFACTORY_CORE_BLOCKINFORMATION_H
