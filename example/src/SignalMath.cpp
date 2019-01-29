#include "SignalMath.h"

#include <BlockFactory/Core/Log.h>
#include <BlockFactory/Core/Parameter.h>
#include <BlockFactory/Core/Signal.h>

using namespace example;

unsigned SignalMath::numberOfParameters()
{
    // The base blockfactory::core::Block class needs parameters (e.g. the ClassName).
    // You must specify here how many more parameters this class needs.
    // Our example needs just one more: the operation to perform.
    return Block::numberOfParameters() + 1;
}

// This method should let BlockInformation know the parameters metadata.
// BlockFactory will use this information to gather the parameters from the active engine.
bool SignalMath::parseParameters(blockfactory::core::BlockInformation* blockInfo)
{
    // Initialize information for our only parameter
    int rows = 1;
    int cols = 1;
    std::string name = "Operation"; // This label is used later to access the paramemeter
    unsigned index = Block::numberOfParameters(); // Indices start from 0
    auto type = blockfactory::core::ParameterType::STRING;

    // Create the parameter
    blockfactory::core::ParameterMetadata parameterMetadata(type, index, rows, cols, name);

    // Add the parameter metadata into the BlockInformation
    if (!blockInfo->addParameterMetadata(parameterMetadata)) {
        bfError << "Failed to store parameter metadata";
        return false;
    }

    // Ask to the BlockInformation interface to parse the parameters and store them into
    // the protected m_parameters member of the parent blockfactory::core::Block class.
    bool paramParsedOk = blockInfo->parseParameters(m_parameters);

    // Return the outcome of the parameter parsing.
    // If the parsing fails, the execution stops.
    return paramParsedOk;
}

// Keep in mind that after this step, all the allocated memory will be deleted.
// Memory persistency is guaranteed starting from the initialize() method.
bool SignalMath::configureSizeAndPorts(blockfactory::core::BlockInformation* blockInfo)
{
    // The base blockfactory::core::Block class needs to be configured first
    if (!blockfactory::core::Block::configureSizeAndPorts(blockInfo)) {
        return false;
    }

    // Create object that store input and output ports information
    blockfactory::core::Port::Info input1{/*portIndex=*/0,
                                          std::vector<int>{blockfactory::core::Port::DynamicSize},
                                          blockfactory::core::Port::DataType::DOUBLE};

    blockfactory::core::Port::Info input2{/*portIndex=*/1,
                                          std::vector<int>{blockfactory::core::Port::DynamicSize},
                                          blockfactory::core::Port::DataType::DOUBLE};

    blockfactory::core::Port::Info output{/*portIndex=*/0,
                                          std::vector<int>{blockfactory::core::Port::DynamicSize},
                                          blockfactory::core::Port::DataType::DOUBLE};

    // Store together the port information objects
    blockfactory::core::InputPortsInfo inputPortInfo;
    blockfactory::core::OutputPortsInfo outputPortInfo;

    inputPortInfo.push_back(input1);
    inputPortInfo.push_back(input2);
    outputPortInfo.push_back(output);

    // Store the port information into the BlockInformation
    if (!blockInfo->setPortsInfo(inputPortInfo, outputPortInfo)) {
        bfError << "Failed to configure input / output ports";
        return false;
    }

    return true;
}

bool SignalMath::initialize(blockfactory::core::BlockInformation* blockInfo)
{
    // The base blockfactory::core::Block class need to be initialized first
    if (!Block::initialize(blockInfo)) {
        return false;
    }

    // Parse the parameters
    if (!SignalMath::parseParameters(blockInfo)) {
        bfError << "Failed to parse parameters.";
        return false;
    }

    // Read the Operation parameter and store it as a private member
    std::string operation;
    if (!m_parameters.getParameter("Operation", operation)) {
        bfError << "Failed to parse Operation parameter";
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
        bfError << "Operation " << operation << " not recognized";
        return false;
    }

    // Check that the size of the input signals match
    if (blockInfo->getInputPortWidth(/*index=*/0) != blockInfo->getInputPortWidth(/*index=*/1)) {
        bfError << "Input signals widths do not match";
        return false;
    }

    return true;
}

bool SignalMath::output(const blockfactory::core::BlockInformation* blockInfo)
{
    // Get the input signals
    blockfactory::core::InputSignalPtr input1 = blockInfo->getInputPortSignal(/*index=*/0);
    blockfactory::core::InputSignalPtr input2 = blockInfo->getInputPortSignal(/*index=*/1);

    // Get the output signal
    blockfactory::core::OutputSignalPtr output = blockInfo->getOutputPortSignal(/*index=*/0);

    // Check the signal validity
    if (!input1 || !input2 || !output) {
        bfError << "Signals not valid";
        return false;
    }

    // Check the width of the output signal.
    // This check is recommended for dynamically sized signals since the engine might
    // fail to propagate the right dimensions.
    if (output->getWidth() != input1->getWidth()) {
        bfError << "Output signal has a width of " << output->getWidth()
                << " while input signals have a width of " << input1->getWidth();
        return false;
    }

    // Perform the given operation
    for (size_t i = 0; i < output->getWidth(); ++i) {
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

bool SignalMath::terminate(const blockfactory::core::BlockInformation* /*blockInfo*/)
{
    return true;
}
