/*
 * Copyright (C) 2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * GNU Lesser General Public License v2.1 or any later version.
 */

#include "BlockFactory/Core/Signal.h"

#include <algorithm>
#include <catch2/catch.hpp>
#include <random>

using namespace blockfactory::core;

static std::vector<double> generateRandomVector(size_t size)
{
    std::vector<double> values(size);

    unsigned seed = 42;
    std::default_random_engine engine{seed};

    std::uniform_real_distribution<> dis(0, 100);
    std::generate(values.begin(), values.end(), [&]() { return dis(engine); });
    return values;
}

TEST_CASE("Contiguous Signal", "[Core][Signal]")
{
    // Initialize a contiguous buffer
    size_t size = 10;
    std::vector<double> contiguousBuffer = generateRandomVector(size);

    // Empty contiguous Signal
    Signal signal{Signal::DataFormat::CONTIGUOUS};
    REQUIRE(signal.getPortDataType() == Port::DataType::DOUBLE);
    REQUIRE(signal.getDataFormat() == Signal::DataFormat::CONTIGUOUS);
    REQUIRE(signal.getBuffer<double>() == nullptr);
    REQUIRE_FALSE(signal.isValid());

    // Initialize not-matching buffer type
    REQUIRE_FALSE(signal.initializeBufferFromNonContiguous({}, size));
    REQUIRE_FALSE(signal.initializeBufferFromContiguousZeroCopy({}, size));

    // Initialize the signal
    REQUIRE(signal.initializeBufferFromContiguous(contiguousBuffer.data(), size));
    REQUIRE(signal.getWidth() == size);
    REQUIRE(signal.getBuffer<double>() != nullptr);
    REQUIRE(signal.isValid());

    // The Signal object should have copied the data internally
    REQUIRE(signal.getBuffer<double>() != contiguousBuffer.data());

    // Check that the data stored in the Signal matches the original buffer
    std::vector<double> contiguousBufferCopy(size);
    std::copy(signal.getBuffer<double>(),
              signal.getBuffer<double>() + signal.getWidth(),
              contiguousBufferCopy.data());
    REQUIRE(contiguousBuffer == contiguousBufferCopy);

    // Check that the data matches using get()
    REQUIRE(signal.getWidth() >= 0);
    for (unsigned i = 0; i < static_cast<unsigned>(signal.getWidth()); ++i) {
        REQUIRE(signal.get<double>(i) == Approx(contiguousBuffer[i]));
    }

    // Create another contiguous buffer
    std::vector<double> contiguousBuffer2 = generateRandomVector(size);

    // Substitute the signal data
    for (unsigned i = 0; i < contiguousBuffer2.size(); ++i) {
        REQUIRE(signal.set(i, contiguousBuffer2[i]));
    }
}

TEST_CASE("Non-Contiguous Signal", "[Core][Signal]")
{
    // Initialize a contiguous buffer
    size_t size = 10;
    std::vector<double> contiguousBuffer = generateRandomVector(size);

    // Create a non-contiguous buffer containing a downsampled version of
    // the contiguous buffer
    std::vector<double*> nonContiguousBuffer;
    for (unsigned i = 0; i < contiguousBuffer.size(); i = i + 2) {
        nonContiguousBuffer.push_back(&contiguousBuffer[i]);
    }

    // Empty contiguous Signal
    Signal signal{Signal::DataFormat::NONCONTIGUOUS};
    REQUIRE(signal.getPortDataType() == Port::DataType::DOUBLE);
    REQUIRE(signal.getDataFormat() == Signal::DataFormat::NONCONTIGUOUS);
    REQUIRE(signal.getBuffer<double>() == nullptr);
    REQUIRE_FALSE(signal.isValid());

    // Initialize not-matching buffer type
    REQUIRE_FALSE(signal.initializeBufferFromContiguous({}, size));
    REQUIRE_FALSE(signal.initializeBufferFromContiguousZeroCopy({}, size));

    // Initialize the signal.
    // This is the type of simulink buffers for non-contiguous input signals.
    auto simulink_ptr = reinterpret_cast<void**>(nonContiguousBuffer.data());
    REQUIRE(signal.initializeBufferFromNonContiguous(simulink_ptr, size));
    REQUIRE(signal.getWidth() == size);
    REQUIRE(signal.getBuffer<double>() != nullptr);
    REQUIRE(signal.isValid());

    // The Signal object should have copied the data internally
    REQUIRE(signal.getBuffer<double>() != contiguousBuffer.data());

    // Check that the data stored in the Signal matches the original buffer
    std::vector<double> contiguousBufferCopy(size);
    std::copy(signal.getBuffer<double>(),
              signal.getBuffer<double>() + signal.getWidth(),
              contiguousBufferCopy.data());
    REQUIRE(contiguousBuffer == contiguousBufferCopy);

    // Check that the data matches using get()
    REQUIRE(signal.getWidth() >= 0);
    for (unsigned i = 0; i < static_cast<unsigned>(signal.getWidth()); ++i) {
        REQUIRE(signal.get<double>(i) == Approx(contiguousBuffer[i]));
    }
}

TEST_CASE("Contiguous Zero-Copy Signal", "[Core][Signal]")
{
    // Initialize a contiguous buffer
    size_t size = 10;
    std::vector<double> contiguousBuffer = generateRandomVector(size);

    // Empty contiguous Signal
    Signal signal{Signal::DataFormat::CONTIGUOUS_ZEROCOPY};
    REQUIRE(signal.getPortDataType() == Port::DataType::DOUBLE);
    REQUIRE(signal.getDataFormat() == Signal::DataFormat::CONTIGUOUS_ZEROCOPY);
    REQUIRE(signal.getBuffer<double>() == nullptr);
    REQUIRE_FALSE(signal.isValid());

    // Initialize not-matching buffer type
    REQUIRE_FALSE(signal.initializeBufferFromContiguous({}, size));
    REQUIRE_FALSE(signal.initializeBufferFromNonContiguous({}, size));

    /// Initialize the signal.
    REQUIRE(signal.initializeBufferFromContiguousZeroCopy(contiguousBuffer.data(), size));
    REQUIRE(signal.getWidth() == size);
    REQUIRE(signal.getBuffer<double>() != nullptr);
    REQUIRE(signal.isValid());

    // The Signal object should not have copied the data internally
    REQUIRE(signal.getBuffer<double>() == contiguousBuffer.data());

    // Check that the data stored in the Signal matches the original buffer
    std::vector<double> contiguousBufferCopy(size);
    std::copy(signal.getBuffer<double>(),
              signal.getBuffer<double>() + signal.getWidth(),
              contiguousBufferCopy.data());
    REQUIRE(contiguousBuffer == contiguousBufferCopy);

    // Check that the data matches using get()
    REQUIRE(signal.getWidth() >= 0);
    for (unsigned i = 0; i < static_cast<unsigned>(signal.getWidth()); ++i) {
        REQUIRE(signal.get<double>(i) == Approx(contiguousBuffer[i]));
    }

    // Change the data of the contiguous buffer without modifying its location in memory
    std::vector<double> newContiguousBuffer = generateRandomVector(size);
    std::copy(newContiguousBuffer.begin(), newContiguousBuffer.end(), contiguousBuffer.data());

    // Check that the Signal object points to the new data
    for (unsigned i = 0; i < static_cast<unsigned>(signal.getWidth()); ++i) {
        REQUIRE(signal.get<double>(i) == Approx(contiguousBuffer[i]));
    }
}
