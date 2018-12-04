/*
 * Copyright (C) 2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * GNU Lesser General Public License v2.1 or any later version.
 */

#ifndef WBT_SIGNAL_H
#define WBT_SIGNAL_H

#include <memory>

namespace wbt {
    class Signal;
    enum class DataType;
} // namespace wbt

/**
 * @brief Defines allowed signal data types
 *
 * This enum defines the data types of signals that are handled by this toolbox.
 *
 * @note Currently only `DOUBLE` is fully implemented.
 * @see Signal::Signal, BlockInformation::setInputPortType, BlockInformation::setOutputPortType
 */
enum class wbt::DataType
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
 * @brief Class to represent data shared between blocks, labelled as signals.
 *
 * Analogously to the block-algorithm corrispondence, this class introduces the signal-data
 * corrispondence. Signals are basically the connections between blocks.
 *
 * Signals do not directly translate to block's input and output. Signals are plugged to block
 * ports, and this block port fill the signal with data.
 *
 * @remark A signal can be plugged to more than one block port.
 * @see wbt::Block
 */
class wbt::Signal
{
public:
    /// Defines the format of signals supported by Signal. It specifies what kind of data the
    /// Signal::m_bufferPtr points.
    ///
    /// - `NONCONTIGUOUS` matches the default Simulink input signals. Signal::m_bufferPtr is a
    ///   pointer to an array of pointers, each of them storing an element of the signal.
    /// - `CONTIGUOUS` means that the Signal::m_bufferPtr points to a contiguous array of
    ///   Signal::m_portDataType type.
    /// - `CONTIGUOUS_ZEROCOPY` matches the default Simulink output signals. Signal::m_bufferPtr is
    ///   a pointer to an _external_ array of Signal::m_portDataType type.
    ///
    /// @note `CONTIGUOUS_ZEROCOPY` is the only format that doesn't copy data from the original
    ///       buffer address. Instead, both `CONTIGUOUS` and `NONCONTIGUOUS` data formats copy
    ///       the content of the buffer inside the Signal object. For performance reason, prefer
    ///       using `CONTIGUOUS_ZEROCOPY`.
    /// @see initializeBufferFromContiguous, initializeBufferFromContiguousZeroCopy,
    ///      initializeBufferFromNonContiguous
    /// @see BlockInformation::getInputPortSignal, BlockInformation::getOutputPortSignal
    enum class DataFormat
    {
        NONCONTIGUOUS = 0,
        CONTIGUOUS = 1,
        CONTIGUOUS_ZEROCOPY = 2
    };

private:
    class impl;
    std::unique_ptr<impl> pImpl;

public:
    enum
    {
        DynamicSize = -1
    };

    Signal(const DataFormat& dataFormat = DataFormat::CONTIGUOUS_ZEROCOPY,
           const DataType& dataType = DataType::DOUBLE);
    ~Signal();

    Signal(const Signal& other);
    Signal& operator=(const Signal& other) = delete;

    Signal(Signal&& other);
    Signal& operator=(Signal&& other) = delete;

    /**
     * @brief Initialize the signal from a contiguous buffer
     *
     * This method allocates a new array with the same size of `buffer` and copies the data. In this
     * case, the Signal object will own the data.
     *
     * @param buffer The pointer to the original contiguous buffer.
     * @return True for success, false otherwise.
     * @see Signal::DataFormat
     */
    bool initializeBufferFromContiguous(const void* buffer);

    /**
     * @brief Initialize the signal from a contiguous buffer without copying data
     *
     * This methods accepts an external contiguous buffer and holds its pointer. The data is not
     * owned by this object.
     *
     * @note You must set the signal width with Signal::setWidth in order to have a valid signal.
     *
     * @param buffer The pointer to the original contiguous buffer.
     * @return True for success, false otherwise.
     * @see Signal::DataFormat
     */
    bool initializeBufferFromContiguousZeroCopy(const void* buffer);

    /**
     * @brief Initialize the signal from a non-contiguous buffer
     *
     * This method allocates a new array with the same size of `bufferPtrs` and copies the data. In
     * this case, the Signal object will own the data. `bufferPtrs` points to an array of pointers.
     * Each of these pointers points to a data element.
     *
     * @note You must set the signal width with Signal::setWidth in order to have a valid signal.
     *
     * @param bufferPtrs The pointer to the original non-contiguous buffer.
     * @return True for success, false otherwise.
     * @see Signal::DataFormat
     */
    bool initializeBufferFromNonContiguous(const void* const* bufferPtrs);

    /**
     * @brief Check if the signal is valid
     *
     * Checks if Signal::m_bufferPtr is not `nullptr` and Signal::m_width is greater than zero.
     *
     * @return True for valid signal, false otherwise.
     */
    bool isValid() const;

    /**
     * @brief Read the width of the signal
     *
     * By default the width of Signal is Signal::DynamicSize. However, for being a valid signal, an
     * object must have a specified width.
     *
     * @return The signal width.
     * @see Signal::setWidth, Signal::isValid
     */
    int getWidth() const;

    /**
     * @brief Read the wbt::DataType of the signal
     *
     * The default type is DataType::DOUBLE.
     *
     * @return The signal data type.
     */
    DataType getPortDataType() const;

    /**
     * @brief Read the Signal::DataFormat of the signal
     *
     * The default type is DataFormat::CONTIGUOUS_ZEROCOPY.
     *
     * @return The signal data format.
     */
    DataFormat getDataFormat() const;

    /**
     * @brief Get the pointer to the buffer storing signal's data
     *
     * The buffer is stored as a void pointer in Signal::m_bufferPtr. In order to use the buffer it
     * should be properly cast to the right data type. Be sure that the Signal::DataType match the
     * type of the buffer otherwise pointer arithmetics does not work.
     *
     * If `T` does not match Signal::m_portDataType the returned value is a `nullptr`.
     *
     * @note Always check if the pointer is not `nullptr` before using it.
     * @tparam The data type of the returned buffer.
     * @return The pointer to the buffer if the class is properly configured, `nullptr` otherwise.
     * @see Signal::setBuffer
     */
    template <typename T>
    T* getBuffer();

    /**
     * @brief Get the pointer to the buffer storing signal's data
     *
     * Documented in Signal::getBuffer
     *
     */
    template <typename T>
    const T* getBuffer() const;

    /**
     * @brief Get a single element of the signal
     *
     * This method returns the `i-th` element of the handled buffer.
     *
     * @note It is recommended to use Signal::isValid before using this method.
     * @tparam The data type of the returned signal. It must match Signal::m_portDataType.
     * @param i The index of the element. It should not exceed Signal::m_width.
     * @return The `i-th` element of the vector if the signal is valid, or the default value of the
     *         type otherwise.
     */
    template <typename T>
    T get(const unsigned i) const;

    /**
     * @brief Set the width of the signal
     *
     * @param width The width to set.
     */
    void setWidth(const unsigned width);

    /**
     * @brief Set the value of a sigle element of the buffer
     *
     * @param index The index of the element to write.
     * @param data The content of the data to write.
     * @return True for success, false otherwise.
     *
     * @todo Port this to a template
     */
    bool set(const unsigned index, const double data);

    /**
     * @brief Set the pointer to the buffer storing signal's data
     *
     * This method allows changing the handled buffer. In the DataFormat::CONTIGUOUS case the data
     * is copied inside the object. Instead, in the DataFormat::CONTIGUOUS_ZEROCOPY only the pointer
     * to the buffer is changed.
     *
     * This method is not allowed for DataFormat::NONCONTIGUOUS format.
     *
     * @tparam The data type of the new buffer.
     * @param data The new buffer address.
     * @param length The size of the new buffer.
     * @return True if the buffer was set sucessfully, false otherwise.
     */
    template <typename T>
    bool setBuffer(const T* data, const unsigned length);
};

// Explicit declaration of templates for all the supported types
// =============================================================

// TODO: for the time being, only DOUBLE is allowed. The toolbox has an almost complete support to
//       many other data types, but they need to be tested.

namespace wbt {
    // DataType::DOUBLE
    extern template double* Signal::getBuffer<double>();
    extern template const double* Signal::getBuffer<double>() const;
    extern template double Signal::get<double>(const unsigned i) const;
    extern template bool Signal::setBuffer<double>(const double* data, const unsigned length);
} // namespace wbt

#endif // WBT_SIGNAL_H
