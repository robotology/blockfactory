/*
 * Copyright (C) 2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * GNU Lesser General Public License v2.1 or any later version.
 */

#ifndef BLOCKFACTORY_CORE_PORT_H
#define BLOCKFACTORY_CORE_PORT_H

#include <vector>

namespace blockfactory {
    namespace core {
        class Port;
    } // namespace core
} // namespace blockfactory

/**
 * @brief Class that stores port properties
 *
 * Ports are virtual entities associated to blocks, and they provide the interface between a block
 * and a signal, carrying the required information.
 *
 * @note Currently only 1D and 2D ports are supported.
 * @note This class is just a placeholder of information. It might become a concrete class in the
 *       future.
 *
 * @see core::Signal, core::Block
 */
class blockfactory::core::Port
{
public:
    virtual ~Port() = 0;

    /// The 0-based index of a port
    using Index = size_t;

    /// Specifies the dimensions of a port. Only 1D and 2D ports (and hence signals) are currently
    /// supported
    using Dimensions = std::vector<int>;

    /// @brief Identifier of a port with dynamic size
    ///
    /// This usually applied when a port is initialized without knowing its dimesions, and it might
    /// happen before the signal-size propagation by the engine.
    static const int DynamicSize = -1;

    /**
     * @brief Defines allowed port data types
     *
     * The port data type specifies which kind of core::Signal object can be plugged into
     * a port.
     *
     * @note Currently only `DOUBLE` is fully implemented.
     * @see core::Signal::Signal,
     *      core::BlockInformation::setInputPortType,
     *      core::BlockInformation::setOutputPortType
     */
    enum class DataType
    {
        DOUBLE,
        SINGLE,
        INT8,
        UINT8,
        INT16,
        UINT16,
        INT32,
        UINT32,
        BOOLEAN,
    };

    /**
     * @brief A container of port information
     */
    struct Info
    {
        /// The 0-based index of a port
        Index index;

        /// The dimensions of a port. If the dimension is set to be dynamic it assumes
        /// the value of Port::DynamicSize.
        Dimensions dimension;

        /// The datatype of a port. It defines the data support of the signals that can be
        /// connected.
        /// @see Port::DataType
        DataType dataType;
    };

    /**
     * @brief Contained of size-related properties of ports
     */
    struct Size
    {
        using Vector = int;

        using Rows = int;
        using Cols = int;
        using Matrix = struct
        {
            Rows rows;
            Cols cols;
        };
    };
};

#endif // BLOCKFACTORY_CORE_PORT_H
