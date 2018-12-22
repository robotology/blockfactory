/*
 * Copyright (C) 2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * GNU Lesser General Public License v2.1 or any later version.
 */

#ifndef BLOCKFACTORY_CORE_BLOCK_H
#define BLOCKFACTORY_CORE_BLOCK_H

#include "BlockFactory/Core/Parameters.h"

#include <string>
#include <vector>

namespace blockfactory {
    namespace core {
        class Block;
        class BlockInformation;
    } // namespace core
} // namespace blockfactory

/**
 * @brief Basic abstract class for wrapping generic algorithms
 *
 * ### Rationale
 *
 * This class is aimed to wrap generic algorithms and it represents the most basic component of
 * `BlockFactory`.
 *
 * The entire execution of a generic algorithm can be split in the following major steps:
 *
 * - Initialization: implemented with core::Block::configureSizeAndPorts, core::Block::initialize
 * - Execution: implemented with core::Block::output
 * - Termination: implemented with core::Block::terminate
 *
 * Considering that the main aim of this framework at its beginning was the integration with
 * Simulink, it contains other methods for exposing the algorithm as a Simulink block.
 *
 * ### Main concept
 *
 * A generic application is the composition of many algorithms sharing and processing each other
 * data. Every algorithm is represented by a core::Block, and the data shared with other blocks is
 * carried by core::Signal.
 *
 * The block is generally unaware of the data that it will process, and it is only interested in
 * knowing how many input / output signals are connected, their size and data type. Blocks have the
 * concept of _ports_, which are the connections between external core::Signal and the block itself.
 *
 * @remark A signal can be plugged to more than one port.
 *
 * This kind of information is set by a core::BlockInformation object, and it is specific to the
 * framework where the algorithm runs (e.g. standalone C++ code, Simulink, etc).
 *
 * Beyond this, an algorithm often needs parameters. This class provides functionalities to gather
 * them in a core::Parameters object.
 *
 * ### Other information
 *
 * You can create a new block by deriving from this class and implementing at least all the abstract
 * methods.
 *
 * @note This block interface assumes that algorithms are represented as instantaneous
 *       systems, there is no default storage capability between different sampling times. However,
 *       concrete blocks can have buffers that retain previous data. \par
 * @note Despite some of the methods inside this class look Simulink-dependent, objects of this
 *       class are completely generic. In fact, core::Block only provides algorithm callbacks, and
 *       the operation to set input / output data is demanded to the core::BlockInformation
 *       interface. For what concerns Simulink, a mex::SimulinkBlockInformation implementation is
 *       provided. \par
 *
 * @see core::BlockInformation Class for providing information about signals and parameters.
 *
 * @section block_parameters Block Parameters
 *
 * | Type | Index | Rows | Cols | Name |
 * | ---- | :---: | :--: | :--: | ---- |
 * | ParameterType::STRING | 0 | 1 | 1 | "className" |
 * | ParameterType::STRING | 1 | 1 | 1 | "libName"   |
 */
class blockfactory::core::Block
{
protected:
    /// Container for block parameters. You can get this member using Block::getParameters
    Parameters m_parameters;

public:
    /**
     * @brief Destructor
     */
    virtual ~Block() = default;

    /**
     * @brief Number of parameters of core::Block
     *
     * Static variable matching Block::numberOfParameters. It might be useful to define
     * parametric constants for parameter indices in child blocks.
     */
    static constexpr unsigned NumberOfParameters = 2;

    /**
     * @brief Returns the number of configuration parameters needed by this block
     *
     * @return The number of parameters.
     *
     * @see Block::NumberOfParameters
     */
    virtual unsigned numberOfParameters();

    /**
     * @brief Returns the vector of additional block options
     *
     * Implement this method if you want to store in the Block additional options that can be parsed
     * later from BlockInformation::optionFromKey.
     *
     * @return A vector containing a list of options.
     */
    virtual std::vector<std::string> additionalBlockOptions();

    /**
     * @brief Returns the number of discrete states of the block
     *
     * The base implementation returns 0, i.e. no discrete states.
     *
     * @note If you return a number > 0, you should implement the Block::updateDiscreteState
     *       function.
     * @return The number of discrete states.
     */
    virtual unsigned numberOfDiscreteStates();

    /**
     * @brief Returns the number of continuous states of the block
     *
     * The base implementation returns 0, i.e. no continuous states.
     *
     * @note If you return a number > 0, you should implement the Block::stateDerivative function.
     * @return The number of continuous states.
     */
    virtual unsigned numberOfContinuousStates();

    /**
     * @brief Update the internal discrete state
     *
     * i.e. `x[i+1] = f(x[i])`
     *
     * @param blockInfo The pointer to a BlockInformation object.
     * @return True for success, false otherwise.
     */
    virtual bool updateDiscreteState(const BlockInformation* blockInfo);

    /**
     * @brief Update the internal continuous state
     *
     * @param blockInfo The pointer to a BlockInformation object.
     * @return True for success, false otherwise.
     */
    virtual bool stateDerivative(const BlockInformation* blockInfo);

    /**
     * @brief Specify if the parameter at the specified index is tunable
     *
     * Tunable means that it can be changed during the simulation. Usually parameters are defined
     * before the beginning of the simulation and they stay constant for all its duration.
     *
     * @note For the time being tunable parameters are not used in this toolbox.
     *
     * @param index Index of the parameter.
     * @return True if the parameter is tunable, false otherwise.
     */
    virtual bool parameterAtIndexIsTunable(unsigned index);

    /**
     * @brief Parse the parameters stored into the core::BlockInformation object
     *
     * Implement this method to create the metadata of the parameters your block needs (using
     * core::ParameterMetadata), store them into the blockInfo object with
     * core::BlockInformation::addParameterMetadata, and parse them using
     * core::BlockInformation::parseParameters.
     *
     * An example of the implementation is the following:
     *
     * ```cpp
     * bool MyBlock::parseParameters(BlockInformation* blockInfo)
     * {
     *     ParameterMetadata fooMetadata(PARAM_STRING, PARAM_IDX_FRAME, 1, 1, "foo");
     *     bool ok = blockInfo->addParameterMetadata(fooMetadata);
     *
     *     if (!ok) {
     *         bfError << "Failed to store parameter metadata.";
     *         return false;
     *     }

     *     return blockInfo->parseParameters(m_parameters);
     * }
     * ```
     *
     * @param blockInfo The pointer to a BlockInformation object.
     * @return True for success, false otherwise.
     * @see core::BlockInformation::addParameterMetadata, core::BlockInformation::parseParameters
     */
    virtual bool parseParameters(BlockInformation* blockInfo);

    /**
     * @brief Gather all the stored parameters
     *
     * After the parameters have been successfully stored and parsed using the
     * core::Block::parseParameter, you can gather them using this method.
     *
     * @param[out] params A core::Parameters object containing block parameters.
     * @return bool True for success, false otherwise.
     */
    bool getParameters(blockfactory::core::Parameters& params) const;

    /**
     * @brief Configure the input and output ports
     *
     * Implement this method to set information about number and size of input and output ports.
     * The terminology `port` comes as Simulink inheritage, and it marks the connection of a signal
     * (which resides in some buffer in the program memory) to the block input or output.
     *
     * These information will be used later (e.g. in the core::Block::initialize and
     * core::Block::output) for preallocating resources and accessing data knowing its size in
     * advance.
     *
     * @note If the size is not known (core::Signal::DynamicSize) during this configuring phase, at
     * latest it should be set in the core::Block::initialize method.
     * @warning Do not allocate any data in this stage! Object are destroyed afterwards and created
     *          again before the core::Block::initialize step. All allocated memory and stored
     *          values will be deleted. Only information stored in `blockInfo` will persist.
     *
     * @param blockInfo The pointer to a BlockInformation object.
     * @return True if the block was configured successfully, false otherwise.
     * @see core::BlockInformation::setNumberOfInputPorts,
     *      core::BlockInformation::setInputPortVectorSize
     */
    virtual bool configureSizeAndPorts(BlockInformation* blockInfo);

    /**
     * @brief Never called.
     *
     * @param blockInfo The pointer to a BlockInformation object.
     * @return True for success, false otherwise.
     */
    virtual bool checkParameters(const BlockInformation* blockInfo);

    /**
     * @brief Initialize the block
     *
     * Implement this method to initialize and allocate the resources the algorithm needs during its
     * execution.
     *
     * @param blockInfo The pointer to a BlockInformation object.
     * @return True for success, false otherwise.
     */
    virtual bool initialize(BlockInformation* blockInfo);

    /**
     * @brief Initialize block initial conditions
     *
     * Implement this method to specify block initial conditions. Its execution will happen after
     * block::Block::initialize and before the first call of core::Block::output. The default is an
     * empty implementation.
     *
     * @note This function is also called on a reset event. In Simulink, an example is when the
     *       block resides in an enabled subsystem.
     *
     * @param blockInfo The pointer to a BlockInformation object.
     * @return True for success, false otherwise.
     */
    virtual bool initializeInitialConditions(const BlockInformation* blockInfo);

    /**
     * @brief Cleanup block resources
     *
     * This method is called during the termination of the execution. Implement this method to
     * deallocate all the memory requested during the previous steps or to perform other terminating
     * operations.
     *
     * @param blockInfo The pointer to a BlockInformation object.
     * @return True for success, false otherwise.
     */
    virtual bool terminate(const BlockInformation* blockInfo);

    /**
     * @brief Compute the output of the block
     *
     * This method is called at every iteration of the model. Implement here a single step of the
     * algorithm.
     *
     * @param blockInfo The pointer to a BlockInformation object.
     * @return True for success, false otherwise.
     */
    virtual bool output(const BlockInformation* blockInfo) = 0;
};

#endif // BLOCKFACTORY_CORE_BLOCK_H
