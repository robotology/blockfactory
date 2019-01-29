/*
 * Copyright (C) 2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * GNU Lesser General Public License v2.1 or any later version.
 */

#include "BlockFactory/Core/Signal.h"
#include "BlockFactory/Core/Log.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <map>
#include <ostream>
#include <typeinfo>

using namespace blockfactory::core;

void Signal::allocateBuffer(const void* const bufferInput, void*& bufferOutput, size_t length)
{
    if (m_dataFormat == DataFormat::CONTIGUOUS_ZEROCOPY) {
        bfWarning << "Trying to allocate a buffer with a non-supported "
                  << "CONTIGUOUS_ZEROCOPY data format.";
        return;
    }

    switch (m_portDataType) {
        case Port::DataType::DOUBLE: {
            // Allocate the array
            bufferOutput = static_cast<void*>(new double[length]);
            // Cast to double
            const double* const bufferInputDouble = static_cast<const double*>(bufferInput);
            double* bufferOutputDouble = static_cast<double*>(bufferOutput);
            // Copy data
            std::copy(bufferInputDouble, bufferInputDouble + length, bufferOutputDouble);
            return;
        }
        default:
            // TODO: Implement other DataType
            bfError << "The specified DataType is not yet supported. Used DOUBLE instead.";
            return;
    }
}

void Signal::deleteBuffer()
{
    if (m_dataFormat == DataFormat::CONTIGUOUS_ZEROCOPY || !m_bufferPtr) {
        return;
    }

    switch (m_portDataType) {
        case Port::DataType::DOUBLE:
            delete[] static_cast<double*>(m_bufferPtr);
            m_bufferPtr = nullptr;
            return;
        default:
            // TODO: Implement other DataType
            bfError << "The specified DataType is not yet supported. Used DOUBLE instead.";
            return;
    }
}

// ======
// SIGNAL
// ======

Signal::~Signal()
{
    deleteBuffer();
}

Signal::Signal(const Signal& other)
    : m_width(other.m_width)
    , m_portDataType(other.m_portDataType)
    , m_dataFormat(other.m_dataFormat)
    , m_bufferPtr(other.m_bufferPtr)
{
    if (m_bufferPtr) {
        switch (m_dataFormat) {
            case DataFormat::CONTIGUOUS_ZEROCOPY:
                // We just need the buffer pointer, which has been already copied
                break;
            case DataFormat::NONCONTIGUOUS:
            case DataFormat::CONTIGUOUS:
                // Copy the allocated data
                allocateBuffer(other.m_bufferPtr, m_bufferPtr, other.m_width);
                break;
        }
    }
}

Signal::Signal(const DataFormat& dataFormat, const Port::DataType& dataType)
    : m_portDataType(dataType)
    , m_dataFormat(dataFormat)
{}

Signal::Signal(Signal&& other)
    : m_width(other.m_width)
    , m_portDataType(other.m_portDataType)
    , m_dataFormat(other.m_dataFormat)
    , m_bufferPtr(other.m_bufferPtr)
{
    other.m_width = 0;
    other.m_bufferPtr = nullptr;
}

bool Signal::initializeBufferFromContiguousZeroCopy(const void* buffer, size_t len)
{
    if (m_dataFormat != DataFormat::CONTIGUOUS_ZEROCOPY) {
        bfError << "Trying to initialize a CONTIGUOUS_ZEROCOPY signal but the configured "
                << "DataFormat does not match.";
        return false;
    }

    if (!buffer) {
        bfError << "Failed to initialize Signal. The pointer to the buffer contains a nullptr";
        return false;
    }

    if (len == 0) {
        bfError << "Failed to initialize the Signal buffer with a length of 0";
        return false;
    }

    // Store the length
    m_width = len;

    // Store the buffer
    m_bufferPtr = const_cast<void*>(buffer);

    return true;
}

bool Signal::initializeBufferFromContiguous(const void* buffer, size_t len)
{
    if (m_dataFormat != DataFormat::CONTIGUOUS) {
        bfError << "Trying to initialize a CONTIGUOUS signal but the configured "
                << "DataFormat does not match.";
        return false;
    }

    if (!buffer) {
        bfError << "Failed to initialize Signal. The pointer to the buffer contains a nullptr";
        return false;
    }

    if (len == 0) {
        bfError << "Failed to initialize the Signal buffer with a length of 0";
        return false;
    }

    // Store the length
    m_width = len;

    // Copy data from the external contiguous buffer to the internal buffer
    allocateBuffer(buffer, m_bufferPtr, m_width);

    return true;
}

bool Signal::initializeBufferFromNonContiguous(const void* const* bufferPtrs, size_t len)
{
    if (m_dataFormat != DataFormat::NONCONTIGUOUS) {
        bfError << "Trying to initialize a NONCONTIGUOUS signal but the configured "
                << "DataFormat does not match.";
        return false;
    }

    if (!bufferPtrs) {
        bfError << "Failed to initialize Signal. The pointer to the buffer contains a nullptr";
        return false;
    }

    if (len == 0) {
        bfError << "Failed to initialize the Signal buffer with a length of 0";
        return false;
    }

    // Store the length
    m_width = len;

    if (m_portDataType == Port::DataType::DOUBLE) {
        // Allocate a new vector to store data from the non-contiguous signal
        m_bufferPtr = static_cast<void*>(new double[m_width]);
        double* bufferPtrDouble = static_cast<double*>(m_bufferPtr);

        // Copy data from MATLAB's memory to the Signal object
        for (size_t i = 0; i < m_width; ++i) {
            const double* valuePtr = static_cast<const double*>(*bufferPtrs);
            bufferPtrDouble[i] = valuePtr[i];
        }
    }
    return true;
}

bool Signal::isValid() const
{
    return m_bufferPtr && (m_width > 0);
}

size_t Signal::getWidth() const
{
    return m_width;
}

Port::DataType Signal::getPortDataType() const
{
    return m_portDataType;
}

Signal::DataFormat Signal::getDataFormat() const
{
    return m_dataFormat;
}

bool Signal::set(const size_t index, const double data)
{
    if (m_width <= index) {
        bfError << "The signal index exceeds its width.";
        return false;
    }

    if (!m_bufferPtr) {
        bfError << "The pointer to data is null. The signal was not configured properly.";
        return false;
    }

    switch (m_portDataType) {
        case Port::DataType::DOUBLE: {
            double* buffer = static_cast<double*>(m_bufferPtr);
            buffer[index] = data;
            break;
        }
        case Port::DataType::SINGLE: {
            float* buffer = static_cast<float*>(m_bufferPtr);
            buffer[index] = static_cast<float>(data);
            break;
        }
        default:
            // TODO: Implement other DataType
            bfError << "The specified DataType is not yet supported. Used DOUBLE instead.";
            return false;
    }
    return true;
}

// Explicit template instantiations
// ================================

namespace blockfactory {
    namespace core {
        template double* Signal::getBuffer<double>();
        template const double* Signal::getBuffer<double>() const;
        template double Signal::get<double>(const size_t i) const;
        template bool Signal::setBuffer<double>(const double* data, const size_t length);
        template double* Signal::getBufferImpl() const;
    } // namespace core
} // namespace blockfactory

// Template definitions
// ===================

template <typename T>
T Signal::get(const size_t i) const
{
    const T* buffer = getBuffer<T>();

    if (!buffer) {
        bfError << "The buffer inside the signal has not been initialized properly.";
        return {};
    }

    if (i >= m_width) {
        bfError << "Trying to access an element that exceeds signal width.";
        return {};
    }

    return buffer[i];
}

template <typename T>
T* Signal::getBufferImpl() const
{
    const std::map<Port::DataType, size_t> mapDataTypeToHash = {
        {Port::DataType::DOUBLE, typeid(double).hash_code()},
        {Port::DataType::SINGLE, typeid(float).hash_code()},
        {Port::DataType::INT8, typeid(int8_t).hash_code()},
        {Port::DataType::UINT8, typeid(uint8_t).hash_code()},
        {Port::DataType::INT16, typeid(int16_t).hash_code()},
        {Port::DataType::UINT16, typeid(uint16_t).hash_code()},
        {Port::DataType::INT32, typeid(int32_t).hash_code()},
        {Port::DataType::UINT32, typeid(uint32_t).hash_code()},
        {Port::DataType::BOOLEAN, typeid(bool).hash_code()}};

    if (!m_bufferPtr) {
        bfError << "The pointer to data is null. The signal was not configured properly.";
        return nullptr;
    }

    // Check the returned matches the same type of the portType.
    // If this is not met, applying pointer arithmetics on the returned
    // pointer would show unknown behaviour.
    if (typeid(T).hash_code() != mapDataTypeToHash.at(m_portDataType)) {
        bfError << "Trying to get the buffer using a type different than its DataType";
        return nullptr;
    }

    // Cast pointer and return it
    return static_cast<T*>(m_bufferPtr);
}

template <typename T>
T* Signal::getBuffer()
{
    return getBufferImpl<T>();
}

template <typename T>
const T* Signal::getBuffer() const
{
    return getBufferImpl<T>();
}

template <typename T>
bool Signal::setBuffer(const T* data, const size_t length)
{
    // Non contiguous signals follow the Simulink convention of being read-only.
    // They are used only for input signals.
    if (m_dataFormat == DataFormat::NONCONTIGUOUS) {
        bfError << "Changing buffer address to NONCONTIGUOUS is not allowed.";
        return false;
    }

    // Fail if the length is greater of the signal width
    if (m_dataFormat == DataFormat::CONTIGUOUS_ZEROCOPY && length > m_width) {
        bfError << "Trying to set a buffer with a length greater than the signal width.";
        return false;
    }

    // Check that T matches the type of raw buffer stored. Use getBuffer since it will return
    // nullptr if this is not met.
    if (!getBuffer<T>()) {
        bfError << "Trying to get a pointer with a type not matching the signal's DataType.";
        return false;
    }

    switch (m_dataFormat) {
        case DataFormat::CONTIGUOUS:
            // Delete the current array
            if (m_bufferPtr) {
                delete getBuffer<T>();
                m_bufferPtr = nullptr;
                m_width = 0;
            }
            // Allocate a new empty array
            m_bufferPtr = static_cast<void*>(new T[length]);
            m_width = length;
            // Fill it with new data
            std::copy(data, data + length, getBuffer<T>());
            break;
        case DataFormat::CONTIGUOUS_ZEROCOPY:
            // Reset current data if width changes
            if (length != m_width) {
                std::fill(getBuffer<T>(), getBuffer<T>() + length, 0);
            }
            // Copy new data
            std::copy(data, data + length, getBuffer<T>());
            // Update the width
            m_width = length;
            break;
        case DataFormat::NONCONTIGUOUS:
            bfError << "The code should never arrive here. Unexpected error.";
            return false;
    }

    return true;
}
