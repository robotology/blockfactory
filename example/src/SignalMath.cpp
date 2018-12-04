#include "SignalMath.h"

#include <Core/Log.h>
#include <Core/Parameter.h>
#include <Core/Signal.h>

using namespace example;

unsigned SignalMath::numberOfParameters()
{
    // The base wbt::Block class needs parameters (e.g. the ClassName).
    // You must specify here how many more parameters this class needs.
    // Our example needs just one more: the operation to perform.
    return Block::numberOfParameters() + 1;
}

// This method should let BlockInformation know the parameters metadata
bool SignalMath::parseParameters(wbt::BlockInformation* blockInfo)
{
    // Initialize information for our only parameter
    int rows = 1;
    int cols = 1;
    std::string name = "Operation"; // This label is used to access the param later
    unsigned index = Block::numberOfParameters(); // Indices start from 0
    wbt::ParameterType type = wbt::ParameterType::STRING;

    // Create the parameter
    wbt::ParameterMetadata parameterMetadata(type, index, rows, cols, name);

    // Add the parameter metadata into the BlockInformation
    if (!blockInfo->addParameterMetadata(parameterMetadata)) {
        wbtError << "Failed to store parameter metadata";
        return false;
    }

    // Ask to the BlockInformation interface to parse the parameters and store them into
    // the m_parameters variable. This variable is contained in the wbt::Block class.
    bool paramParsedOk = blockInfo->parseParameters(m_parameters);

    // Return the outcome of the parameter parsing.
    // If the parsing fails, the execution stops.
    return paramParsedOk;
}

// Keep in mind that after this step, all the allocated memory will be deleted.
// Memory persistency is guaranteed starting from the initialize method.
bool SignalMath::configureSizeAndPorts(wbt::BlockInformation* blockInfo)
{
    // The base wbt::Block class need to be configured
    if (!wbt::Block::configureSizeAndPorts(blockInfo)) {
        return false;
    }

    // Create data about input and output ports.
    wbt::BlockInformation::PortData input1;
    wbt::BlockInformation::PortData input2;
    wbt::BlockInformation::PortData output;
    input1 = {/*portIndex=*/0, std::vector<int>{wbt::Signal::DynamicSize}, wbt::DataType::DOUBLE};
    input2 = {/*portIndex=*/1, std::vector<int>{wbt::Signal::DynamicSize}, wbt::DataType::DOUBLE};
    output = {/*portIndex=*/0, std::vector<int>{wbt::Signal::DynamicSize}, wbt::DataType::DOUBLE};

    // Populate a structure with the overall input / output data
    wbt::BlockInformation::IOData ioData;
    ioData.input.push_back(input1);
    ioData.input.push_back(input2);
    ioData.output.push_back(output);

    // Store this data into the BlockInformation
    if (!blockInfo->setIOPortsData(ioData)) {
        wbtError << "Failed to configure input / output ports";
        return false;
    }

    return true;
}

bool SignalMath::initialize(wbt::BlockInformation* blockInfo)
{
    // The base wbt::Block class need to be initialized
    if (!Block::initialize(blockInfo)) {
        return false;
    }

    // Parse the parameters
    if (!SignalMath::parseParameters(blockInfo)) {
        wbtError << "Failed to parse parameters.";
        return false;
    }

    // Read the Operation parameter and store it as a private member
    std::string operation;
    if (!m_parameters.getParameter("Operation", operation)) {
        wbtError << "Failed to parse Operation parameter";
        return false;
    }

    // Check the content of the parameter
    if (operation == "Addition") {
        m_operation = Operation::ADDITION;
    }
    else if (operation == "Subtraction") {
        m_operation = Operation::SUBTRACTION;
    }
    else if (operation == "Multiplication") {
        m_operation = Operation::MULTIPLICATION;
        ;
    }
    else {
        wbtError << "Operation " << operation << " not supported";
        return false;
    }

    // Check that the size of the input signals match
    if (blockInfo->getInputPortWidth(/*index=*/0) != blockInfo->getInputPortWidth(/*index=*/1)) {
        wbtError << "Input signals widths do not match";
        return false;
    }

    return true;
}

bool SignalMath::output(const wbt::BlockInformation* blockInfo)
{
    // Get the input signals
    wbt::InputSignalPtr input1 = blockInfo->getInputPortSignal(/*index=*/0);
    wbt::InputSignalPtr input2 = blockInfo->getInputPortSignal(/*index=*/1);

    // Get the output signal
    wbt::OutputSignalPtr output = blockInfo->getOutputPortSignal(/*index=*/0);

    // Check the signal validity
    if (!input1 || !input2 || !output) {
        wbtError << "Signals not valid";
        return false;
    }

    // Check the width of the output signal.
    // This is recommended for dynamically sized signals.
    if (output->getWidth() != input1->getWidth()) {
        wbtError << "Output signal has a width of " << output->getWidth()
                 << " while input signals have a width of " << input1->getWidth();
        return false;
    }

    // Perform the given operation
    for (unsigned i = 0; i < output->getWidth(); ++i) {
        switch (m_operation) {
            case Operation::ADDITION:
                output->set(i, input1->get<double>(i) + input2->get<double>(i));
                break;
            case Operation::SUBTRACTION:
                output->set(i, input1->get<double>(i) - input2->get<double>(i));
                break;
            case Operation::MULTIPLICATION:
                output->set(i, input1->get<double>(i) * input2->get<double>(i));
                break;
        }
    }

    return true;
}

bool SignalMath::terminate(const wbt::BlockInformation* /*blockInfo*/)
{
    return true;
}
