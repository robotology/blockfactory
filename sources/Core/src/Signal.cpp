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

// ============
// SIGNAL::IMPL
// ============

class Signal::impl
{
public:
    int width = Signal::DynamicSize;
    const DataType portDataType;
    const DataFormat dataFormat;

    void* bufferPtr = nullptr;

    template <typename T>
    T* getBufferImpl();

    void deleteBuffer();
    void allocateBuffer(const void* const bufferInput, void*& bufferOutput, const unsigned& length);

    impl(const DataFormat& dFormat, const DataType& dType)
        : portDataType(dType)
        , dataFormat(dFormat)
    {}

    impl* clone() { return new impl(*this); }
};

void Signal::impl::allocateBuffer(const void* const bufferInput,
                                  void*& bufferOutput,
                                  const unsigned& length)
{
    if (dataFormat == DataFormat::CONTIGUOUS_ZEROCOPY) {
        bfWarning << "Trying to allocate a buffer with a non-supported "
                  << "CONTIGUOUS_ZEROCOPY data format.";
        return;
    }

    switch (portDataType) {
        case DataType::DOUBLE: {
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

void Signal::impl::deleteBuffer()
{
    if (dataFormat == DataFormat::CONTIGUOUS_ZEROCOPY || !bufferPtr) {
        return;
    }

    switch (portDataType) {
        case DataType::DOUBLE:
            delete static_cast<double*>(bufferPtr);
            bufferPtr = nullptr;
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
    pImpl->deleteBuffer();
}

Signal::Signal(const Signal& other)
    : pImpl{other.pImpl->clone()}
{
    if (pImpl->bufferPtr) {
        switch (pImpl->dataFormat) {
            case DataFormat::CONTIGUOUS_ZEROCOPY:
                // We just need the buffer pointer, which has been already copied
                // by the pImpl clone.
                break;
            case DataFormat::NONCONTIGUOUS:
            case DataFormat::CONTIGUOUS:
                // Copy the allocated data
                pImpl->allocateBuffer(other.pImpl->bufferPtr, pImpl->bufferPtr, other.pImpl->width);
                break;
        }
    }
}

Signal::Signal(const DataFormat& dataFormat, const DataType& dataType)
    : pImpl(std::make_unique<impl>(dataFormat, dataType))
{}

Signal::Signal(Signal&& other)
    : pImpl{other.pImpl->clone()}
{
    other.pImpl->width = 0;
    other.pImpl->bufferPtr = nullptr;
}

bool Signal::initializeBufferFromContiguousZeroCopy(const void* buffer)
{
    if (pImpl->dataFormat != DataFormat::CONTIGUOUS_ZEROCOPY) {
        bfError << "Trying to initialize a CONTIGUOUS_ZEROCOPY signal but the configured "
                << "DataFormat does not match.";
        return false;
    }

    pImpl->bufferPtr = const_cast<void*>(buffer);
    return true;
}

bool Signal::initializeBufferFromContiguous(const void* buffer)
{
    if (pImpl->dataFormat != DataFormat::CONTIGUOUS) {
        bfError << "Trying to initialize a CONTIGUOUS signal but the configured "
                << "DataFormat does not match.";
        return false;
    }

    if (pImpl->width <= 0) {
        bfError << "Signal width unknown. Unable to initialize the buffer if the "
                << "signal size is not set.";
        return false;
    }

    // Copy data from the external contiguous buffer to the internal buffer
    pImpl->allocateBuffer(buffer, pImpl->bufferPtr, pImpl->width);

    return true;
}

bool Signal::initializeBufferFromNonContiguous(const void* const* bufferPtrs)
{
    if (pImpl->dataFormat != DataFormat::NONCONTIGUOUS) {
        bfError << "Trying to initialize a NONCONTIGUOUS signal but the configured "
                << "DataFormat does not match.";
        return false;
    }

    if (pImpl->width <= 0) {
        bfError << "Signal width unknown. Unable to initialize the buffer if the "
                << "signal size is not set.";
        return false;
    }

    if (pImpl->portDataType == DataType::DOUBLE) {
        // Allocate a new vector to store data from the non-contiguous signal
        pImpl->bufferPtr = static_cast<void*>(new double[pImpl->width]);
        double* bufferPtrDouble = static_cast<double*>(pImpl->bufferPtr);

        // Copy data from MATLAB's memory to the Signal object
        for (auto i = 0; i < pImpl->width; ++i) {
            const double* valuePtr = static_cast<const double*>(*bufferPtrs);
            bufferPtrDouble[i] = valuePtr[i];
        }
    }
    return true;
}

bool Signal::isValid() const
{
    return pImpl->bufferPtr && (pImpl->width > 0);
}

void Signal::setWidth(const unsigned width)
{
    pImpl->width = width;
}

int Signal::getWidth() const
{
    return pImpl->width;
}

DataType Signal::getPortDataType() const
{
    return pImpl->portDataType;
}

Signal::DataFormat Signal::getDataFormat() const
{
    return pImpl->dataFormat;
}

bool Signal::set(const unsigned index, const double data)
{
    if (pImpl->width <= index) {
        bfError << "The signal index exceeds its width.";
        return false;
    }

    if (!pImpl->bufferPtr) {
        bfError << "The pointer to data is null. The signal was not configured properly.";
        return false;
    }

    switch (pImpl->portDataType) {
        case DataType::DOUBLE: {
            double* buffer = static_cast<double*>(pImpl->bufferPtr);
            buffer[index] = data;
            break;
        }
        case DataType::SINGLE: {
            float* buffer = static_cast<float*>(pImpl->bufferPtr);
            buffer[index] = static_cast<float>(data);
            break;
        }
        default:
            // TODO: Implement other DataType
            bfError << "The specified DataType is not yet supported. Used DOUBLE instead.";
            return false;
            break;
    }
    return true;
}

// Explicit template instantiations
// ================================

namespace blockfactory {
    namespace core {
        template double* Signal::getBuffer<double>();
        template const double* Signal::getBuffer<double>() const;
        template double Signal::get<double>(const unsigned i) const;
        template bool Signal::setBuffer<double>(const double* data, const unsigned length);
    } // namespace core
} // namespace blockfactory

// Template definitions
// ===================

template <typename T>
T Signal::get(const unsigned i) const
{
    const T* buffer = getBuffer<T>();

    if (!buffer) {
        bfError << "The buffer inside the signal has not been initialized properly.";
        return {};
    }

    if (i >= pImpl->width) {
        bfError << "Trying to access an element that exceeds signal width.";
        return {};
    }

    return buffer[i];
}

template <typename T>
T* Signal::impl::getBufferImpl()
{
    const std::map<DataType, size_t> mapDataTypeToHash = {
        {DataType::DOUBLE, typeid(double).hash_code()},
        {DataType::SINGLE, typeid(float).hash_code()},
        {DataType::INT8, typeid(int8_t).hash_code()},
        {DataType::UINT8, typeid(uint8_t).hash_code()},
        {DataType::INT16, typeid(int16_t).hash_code()},
        {DataType::UINT16, typeid(uint16_t).hash_code()},
        {DataType::INT32, typeid(int32_t).hash_code()},
        {DataType::UINT32, typeid(uint32_t).hash_code()},
        {DataType::BOOLEAN, typeid(bool).hash_code()}};

    if (!bufferPtr) {
        bfError << "The pointer to data is null. The signal was not configured properly.";
        return nullptr;
    }

    // Check the returned matches the same type of the portType.
    // If this is not met, applying pointer arithmetics on the returned
    // pointer would show unknown behaviour.
    if (typeid(T).hash_code() != mapDataTypeToHash.at(portDataType)) {
        bfError << "Trying to get the buffer using a type different than its DataType";
        return nullptr;
    }

    // Cast pointer and return it
    return static_cast<T*>(bufferPtr);
}

template <typename T>
T* Signal::getBuffer()
{
    return pImpl->getBufferImpl<T>();
}

template <typename T>
const T* Signal::getBuffer() const
{
    return pImpl->getBufferImpl<T>();
}

template <typename T>
bool Signal::setBuffer(const T* data, const unsigned length)
{
    // Non contiguous signals follow the Simulink convention of being read-only.
    // They are used only for input signals.
    if (pImpl->dataFormat == DataFormat::NONCONTIGUOUS) {
        bfError << "Changing buffer address to NONCONTIGUOUS is not allowed.";
        return false;
    }

    // Fail if the length is greater of the signal width
    if (pImpl->dataFormat == DataFormat::CONTIGUOUS_ZEROCOPY && length > pImpl->width) {
        bfError << "Trying to set a buffer with a length greater than the signal width.";
        return false;
    }

    // Check that T matches the type of raw buffer stored. Use getBuffer since it will return
    // nullptr if this is not met.
    if (!getBuffer<T>()) {
        bfError << "Trying to get a pointer with a type not matching the signal's DataType.";
        return false;
    }

    switch (pImpl->dataFormat) {
        case DataFormat::CONTIGUOUS:
            // Delete the current array
            if (pImpl->bufferPtr) {
                delete getBuffer<T>();
                pImpl->bufferPtr = nullptr;
                pImpl->width = 0;
            }
            // Allocate a new empty array
            pImpl->bufferPtr = static_cast<void*>(new T[length]);
            pImpl->width = length;
            // Fill it with new data
            std::copy(data, data + length, getBuffer<T>());
            break;
        case DataFormat::CONTIGUOUS_ZEROCOPY:
            // Reset current data if width changes
            if (length != pImpl->width) {
                std::fill(getBuffer<T>(), getBuffer<T>() + length, 0);
            }
            // Copy new data
            std::copy(data, data + length, getBuffer<T>());
            // Update the width
            pImpl->width = length;
            break;
        case DataFormat::NONCONTIGUOUS:
            bfError << "The code should never arrive here. Unexpected error.";
            return false;
    }

    return true;
}
