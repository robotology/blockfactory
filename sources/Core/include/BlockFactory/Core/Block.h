/*
 * Copyright (C) 2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * GNU Lesser General Public License v2.1 or any later version.
 */

#ifndef WBT_BLOCK_H
#define WBT_BLOCK_H

#include "BlockFactory/Core/Parameters.h"
#include <string>
#include <vector>

namespace wbt {
    class Block;
    class BlockInformation;
} // namespace wbt

/**
 * @brief Basic abstract class for wrapping generic algorithms
 *
 * ### Rationale
 *
 * This class is aimed to wrap generic algorithms and it represents the most basic component of
 * `WB-Toolbox`.
 *
 * The entire execution of an generic algorithm can be split in the following major steps:
 *
 * - Initialization: implemented with Block::configureSizeAndPorts, Block::initialize
 * - Execution: implemented with Block::output
 * - Termination: implemented with Block::terminate
 *
 * Considering that the main aim of this toolbox at its beginning was its integration with Simulink,
 * it contains other methods for exposing the algorithm as a Simulink block.
 *
 * ### Main concept
 *
 * A generic application is the composition of many algorithms sharing and processing each other
 * data. Every algorithm is represented by a wbt::Block, and the data shared with other blocks is
 * carried by wbt::Signal.
 *
 * The block is generally unaware of the data that it will process, and it is only interested in
 * knowing how many input / output signals are connected, their size and data type. Blocks have the
 * concept of _ports_, which are the connections between external wbt::Signal and the block itself.
 *
 * @remark A signal can be plugged to more than one port.
 *
 * This kind of information is set by a wbt::BlockInformation object, and it can be specific to the
 * framework where the algorithm runs (e.g. standalone C++ code, Simulink, etc).
 *
 * Beyond this, an algorithm often needs parameters. This class provides functionalities to gather
 * them in a wbt::Parameters object.
 *
 * ### Other information
 *
 * You can create a new block by deriving this class and implementing at least all the pure virtual
 * methods.
 *
 * @note This class and the entire toolbox assume that algorithms are represented as instantaneous
 *       systems, there is no default storage capability between different sampling times. However,
 *       blocks can wrap generic classes which can retain data, outsourcing data persistence outside
 *       the framework given by `WB-Toolbox`.
 * \par
 * @note Despite some of the methods inside this class looks Simulink-dependent, objects of this
 *       class are completely generic. In fact, it only provides algorithm callbacks, and the
 *       setting of input / output data is demanded to the wbt::BlockInformation interface. For what
 *       concerns Simulink, a wbt::SimulinkBlockInformation implementation is provided.
 * \par
 *
 * @see wbt::BlockInformation Class for providing information about signals and parameters.
 * @see wbt::WBBlock Specialization of wbt::Block which provides useful resources for developing
 *      whole-body blocks.
 *
 * @section block_parameters Block Parameters
 *
 * | Type | Index | Rows | Cols | Name |
 * | ---- | :---: | :--: | :--: | ---- |
 * | PARAM_STRING | 0 | 1 | 1 | "className" |
 *
 * @see wbt::ParameterMetadata::ParameterMetadata for the data types of these variables.
 */
class wbt::Block
{
protected:
    /// Container for block's parameter. You can get this member using Block::getParameters
    Parameters m_parameters;

public:
    /**
     * @brief Create and returns a new Block object of the specified class
     *
     * If the class does not exist returns `nullptr`.
     *
     * @param blockClassName The derived class name to be instantiated.
     * @return The newly created Block object or `nullptr`.
     */
    //    static wbt::Block* instantiateBlockWithClassName(const std::string& blockClassName);

    /**
     * @brief Destructor
     */
    virtual ~Block() = default;

    /**
     * Static variable matching Block::numberOfParameters. It might be useful to define parametric
     * constants for parameter indices in child blocks.
     *
     * @see WBBlock::NumberOfParameters
     */
    static constexpr unsigned NumberOfParameters = 2;

    /**
     * @brief Returns the number of configuration parameters needed by this block
     *
     * @return The number of parameters.
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
     * @param blockInfo A BlockInformation object.
     * @return True for success, false otherwise.
     */
    virtual bool updateDiscreteState(const BlockInformation* blockInfo);

    /**
     * @brief Update the internal continuous state
     *
     * @param blockInfo The SimStruct structure.
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
     * @param[in]  index   Index of the parameter.
     * @param[out] tunable True if the parameter is tunable, false otherwise.
     */
    virtual void parameterAtIndexIsTunable(unsigned index, bool& tunable);

    /**
     * @brief Parse the parameters stored into the BlockInformation object
     *
     * Implement this method to create the metadata of the parameters your block needs (using
     * wbt::ParameterMetadata), store them into the blockInfo object with
     * BlockInformation::addParameterMetadata, and parse them using
     * BlockInformation::parseParameters.
     *
     * An example of the implementation is the following:
     *
     * ```cpp
     * bool Jacobian::parseParameters(BlockInformation* blockInfo)
     * {
     *     ParameterMetadata frameMetadata(PARAM_STRING, PARAM_IDX_FRAME, 1, 1, "frame");
     *     bool ok = blockInfo->addParameterMetadata(frameMetadata);
     *
     *     if (!ok) {
     *         wbtError << "Failed to store parameters metadata.";
     *         return false;
     *     }

     *     return blockInfo->parseParameters(m_parameters);
     * }
     * ```
     *
     * @param blockInfo The pointer to a BlockInformation object.
     * @return True for success, false otherwise.
     * @see BlockInformation::addParameterMetadata, BlockInformation::parseParameters
     */
    virtual bool parseParameters(BlockInformation* blockInfo);

    /**
     * @brief Gather all the stored parameters
     *
     * After the parameters have been successfully stored and parsed using the
     * Block::parseParameter, you can gather them using this method.
     *
     * @param[out] params A wbt::Parameters object containing block's parameters.
     * @return bool True for success, false otherwise.
     */
    bool getParameters(wbt::Parameters& params) const;

    /**
     * @brief Configure the input and output ports
     *
     * Implement this method to set information about number and size of input and output signals.
     * The terminology `port` comes as Simulink inheritage, and it marks the connection of a signal
     * (which resides in some buffer in the program memory) to the block's input or output.
     *
     * These information can be used later (e.g. in the Block::initialize and Block::output) for
     * preallocating resources and accessing data knowing its size in advance.
     *
     * @note If the size is not known at this stage (Signal::DynamicSize), at latest it should be
     *       set in the Block::initialize step
     * @warning Do not allocate any data in this stage! Object are destroyed afterwards and created
     *          again before the Block::initialize step. Every allocated memory and stored values
     *          will be deleted. Only information stored in `blockInfo` will persist.
     *
     * @param blockInfo The pointer to a BlockInformation object.
     * @return True if the block was configured successfully, false otherwise.
     * @see BlockInformation::setNumberOfInputPorts, BlockInformation::setInputPortVectorSize
     */
    virtual bool configureSizeAndPorts(BlockInformation* blockInfo);

    /**
     * Never called.
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
     * @brief Initialize block's initial conditions
     *
     * Implement this method to specify block's initial conditions. Its execution will happen after
     * Block::initialize and before the first call of Block::output. The default is an empty
     * implementation.
     *
     * @note this function is also called on a reset event. In Simulink, an example is when the
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

#endif // WBT_BLOCK_H
