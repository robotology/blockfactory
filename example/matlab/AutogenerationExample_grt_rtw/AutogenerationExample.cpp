/*
 * AutogenerationExample.cpp
 *
 * Non-Degree Granting Education License -- for use at non-degree
 * granting, nonprofit, educational organizations only. Not for
 * government, commercial, or other organizational use.
 *
 * Code generation for model "AutogenerationExample".
 *
 * Model version              : 1.10
 * Simulink Coder version : 8.14 (R2018a) 06-Feb-2018
 * C++ source code generated on : Tue Jan 29 14:43:33 2019
 *
 * Target selection: grt.tlc
 * Note: GRT includes extra infrastructure and instrumentation for prototyping
 * Embedded hardware selection: Intel->x86-64 (Windows64)
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#include "AutogenerationExample.h"
#include "AutogenerationExample_private.h"

/* Model step function */
void AutogenerationExampleModelClass::step()
{
    /* S-Function (BlockFactory): '<Root>/Signal Math' incorporates:
     *  Constant: '<Root>/Constant'
     *  Inport: '<Root>/Input2'
     *  Outport: '<Root>/Result'
     */
    {
        // Get the CoderBlockInformation from the PWork
        blockfactory::coder::CoderBlockInformation* blockInfo = nullptr;
        blockInfo = static_cast<blockfactory::coder::CoderBlockInformation*>(
            AutogenerationExample_DW.SignalMath_PWORK.blockPWork[1]);

        // Get the Block from the PWork
        blockfactory::core::Block* blockPtr = nullptr;
        blockPtr = static_cast<blockfactory::core::Block*>(
            AutogenerationExample_DW.SignalMath_PWORK.blockPWork[0]);

        // Calculate the output
        // --------------------
        bool ok;
        ok = blockPtr->output(blockInfo);

        // Report errors
        if (!ok) {
            std::string error = blockfactory::core::Log::getSingleton().getErrors();
            error = "[Output]" + error;

            // Trim the message if needed
            if (error.length() >= 1024) {
                error = error.substr(0, 1024 - 1);
            }

            // This shouldn't happen
            if (getRTM()->errorStatus) {
                delete getRTM()->errorStatus;
                getRTM()->errorStatus = nullptr;
            }

            getRTM()->errorStatus = new char[1024];
            sprintf(const_cast<char_T*>(getRTM()->errorStatus), "%s", error.c_str());
            return;
        }
    }

    // End of S-Function Block: <Root>/Signal Math
}

/* Model initialize function */
void AutogenerationExampleModelClass::initialize()
{
    /* Registration code */

    /* initialize error status */
    rtmSetErrorStatus(getRTM(), (NULL));

    /* states (dwork) */
    (void) memset((void*) &AutogenerationExample_DW, 0, sizeof(DW_AutogenerationExample_T));

    /* external inputs */
    (void) memset((void*) &AutogenerationExample_U, 0, sizeof(ExtU_AutogenerationExample_T));

    /* external outputs */
    (void) memset(&AutogenerationExample_Y.Result[0], 0, 5U * sizeof(real_T));

    /* Start for S-Function (BlockFactory): '<Root>/Signal Math' incorporates:
     *  Constant: '<Root>/Constant'
     *  Inport: '<Root>/Input2'
     *  Outport: '<Root>/Result'
     */
    {
        // Create and store the CoderBlockInformation object
        blockfactory::coder::CoderBlockInformation* blockInfo =
            new blockfactory::coder::CoderBlockInformation();
        AutogenerationExample_DW.SignalMath_PWORK.blockPWork[1] = static_cast<void*>(blockInfo);

        // Initialize the parameters
        // -------------------------
        blockfactory::core::Parameters params;
        params.storeParameter<std::string>(
            "Addition",
            blockfactory::core::ParameterMetadata(
                blockfactory::core::ParameterType::STRING, 2.0, 1.0, 1.0, "Operation"));
        params.storeParameter<std::string>(
            "ExampleToolbox",
            blockfactory::core::ParameterMetadata(
                blockfactory::core::ParameterType::STRING, 1.0, 1.0, 1.0, "libName"));
        params.storeParameter<std::string>(
            "SignalMath",
            blockfactory::core::ParameterMetadata(
                blockfactory::core::ParameterType::STRING, 0.0, 1.0, 1.0, "className"));

        // Store the parameters in the CoderBlockInformation object
        blockInfo->storeRTWParameters(params);

        // Initialize input / output Signals
        // ---------------------------------

        // Inputs
        blockInfo->setInputPort({0, {1, 5}, blockfactory::core::Port::DataType::DOUBLE}

                                ,
                                static_cast<void*>(&AutogenerationExample_P.Constant_Value[0]));
        blockInfo->setInputPort({1, {1, 5}, blockfactory::core::Port::DataType::DOUBLE}

                                ,
                                static_cast<void*>(&AutogenerationExample_U.input2[0]));

        // Outputs
        blockInfo->setOutputPort({0, {1, 5}, blockfactory::core::Port::DataType::DOUBLE}

                                 ,
                                 static_cast<void*>(&AutogenerationExample_Y.Result[0]));

        // Initialize the class
        // --------------------

        // Get the block factory
        auto factory = blockfactory::core::ClassFactorySingleton::getInstance().getClassFactory(
            {"ExampleToolbox", "SignalMath"});

        if (!factory) {
            bfError << "Failed to get factory object (className="
                    << "SignalMath"
                    << ",libName="
                    << "ExampleToolbox"
                    << ")";
            std::string error = blockfactory::core::Log::getSingleton().getErrors();
            error = "[Initialize]" + error;

            // Trim the message if needed
            if (error.length() >= 1024) {
                error = error.substr(0, 1024 - 1);
            }

            // This shouldn't happen
            if (getRTM()->errorStatus) {
                delete getRTM()->errorStatus;
                getRTM()->errorStatus = nullptr;
            }

            getRTM()->errorStatus = new char[1024];
            sprintf(const_cast<char_T*>(getRTM()->errorStatus), "%s", error.c_str());
            return;
        }

        if (!factory->isValid()) {
            bfError << "Factory error (" << static_cast<std::uint32_t>(factory->getStatus())
                    << "): " << factory->getError().c_str();
            std::string error = blockfactory::core::Log::getSingleton().getErrors();
            error = "[Initialize]" + error;

            // Trim the message if needed
            if (error.length() >= 1024) {
                error = error.substr(0, 1024 - 1);
            }

            // This shouldn't happen
            if (getRTM()->errorStatus) {
                delete getRTM()->errorStatus;
                getRTM()->errorStatus = nullptr;
            }

            getRTM()->errorStatus = new char[1024];
            sprintf(const_cast<char_T*>(getRTM()->errorStatus), "%s", error.c_str());
            return;
        }

        // Run a dummy configureSizeAndPorts step. This is currently required for properly
        // handling optional input / outputs static variables.
        // TODO: find a better way to handle them.
        {
            // Create a new class. This object will be destroyed at the end of the scope.
            shlibpp::SharedLibraryClass<blockfactory::core::Block> blockPtr(*factory);
            auto tmpCoderBlockInfo = std::unique_ptr<blockfactory::coder::CoderBlockInformation>(
                new blockfactory::coder::CoderBlockInformation);
            tmpCoderBlockInfo->storeRTWParameters(params);
            blockPtr->configureSizeAndPorts(tmpCoderBlockInfo.get());
        }

        // Allocate the block
        blockfactory::core::Block* blockPtr = factory->create();
        factory->addRef();

        // Initialize the block
        bool ok = blockPtr->initialize(blockInfo);

        // Report errors
        if (!ok) {
            std::string error = blockfactory::core::Log::getSingleton().getErrors();
            error = "[Initialize]" + error;

            // Trim the message if needed
            if (error.length() >= 1024) {
                error = error.substr(0, 1024 - 1);
            }

            // This shouldn't happen
            if (getRTM()->errorStatus) {
                delete getRTM()->errorStatus;
                getRTM()->errorStatus = nullptr;
            }

            getRTM()->errorStatus = new char[1024];
            sprintf(const_cast<char_T*>(getRTM()->errorStatus), "%s", error.c_str());
            return;
        }

        // Call the initializeInitialConditions() method
        ok = blockPtr->initializeInitialConditions(blockInfo);

        // Report errors
        if (!ok) {
            std::string error = blockfactory::core::Log::getSingleton().getErrors();
            error = "[InitializeInitialConditions]" + error;

            // Trim the message if needed
            if (error.length() >= 1024) {
                error = error.substr(0, 1024 - 1);
            }

            // This shouldn't happen
            if (getRTM()->errorStatus) {
                delete getRTM()->errorStatus;
                getRTM()->errorStatus = nullptr;
            }

            getRTM()->errorStatus = new char[1024];
            sprintf(const_cast<char_T*>(getRTM()->errorStatus), "%s", error.c_str());
            return;
        }

        // Store the block in the PWork vector
        AutogenerationExample_DW.SignalMath_PWORK.blockPWork[0] = static_cast<void*>(blockPtr);
    }
    // End of S-Function Block: <Root>/Signal Math
}

/* Model terminate function */
void AutogenerationExampleModelClass::terminate()
{
    /* Terminate for S-Function (BlockFactory): '<Root>/Signal Math' incorporates:
     *  Constant: '<Root>/Constant'
     *  Inport: '<Root>/Input2'
     *  Outport: '<Root>/Result'
     */
    {
        // Get the CoderBlockInformation from the PWork
        blockfactory::coder::CoderBlockInformation* blockInfo = nullptr;
        blockInfo = static_cast<blockfactory::coder::CoderBlockInformation*>(
            AutogenerationExample_DW.SignalMath_PWORK.blockPWork[1]);

        // Get the Block from the PWork
        blockfactory::core::Block* blockPtr = nullptr;
        blockPtr = static_cast<blockfactory::core::Block*>(
            AutogenerationExample_DW.SignalMath_PWORK.blockPWork[0]);

        // Get the block factory
        auto factory = blockfactory::core::ClassFactorySingleton::getInstance().getClassFactory(
            {"ExampleToolbox", "SignalMath"});

        if (!factory) {
            bfError << "Failed to get factory object (className="
                    << "SignalMath"
                    << ",libName="
                    << "ExampleToolbox"
                    << ")";
            std::string error = blockfactory::core::Log::getSingleton().getErrors();
            error = "[Initialize]" + error;

            // Trim the message if needed
            if (error.length() >= 1024) {
                error = error.substr(0, 1024 - 1);
            }

            // This shouldn't happen
            if (getRTM()->errorStatus) {
                delete getRTM()->errorStatus;
                getRTM()->errorStatus = nullptr;
            }

            getRTM()->errorStatus = new char[1024];
            sprintf(const_cast<char_T*>(getRTM()->errorStatus), "%s", error.c_str());
            return;
        }

        if (!factory->isValid()) {
            bfError << "Factory error (" << static_cast<std::uint32_t>(factory->getStatus())
                    << "): " << factory->getError().c_str();
            std::string error = blockfactory::core::Log::getSingleton().getErrors();
            error = "[Initialize]" + error;

            // Trim the message if needed
            if (error.length() >= 1024) {
                error = error.substr(0, 1024 - 1);
            }

            // This shouldn't happen
            if (getRTM()->errorStatus) {
                delete getRTM()->errorStatus;
                getRTM()->errorStatus = nullptr;
            }

            getRTM()->errorStatus = new char[1024];
            sprintf(const_cast<char_T*>(getRTM()->errorStatus), "%s", error.c_str());
            return;
        }

        // Terminate the class
        // -------------------
        bool ok;
        ok = blockPtr->terminate(blockInfo);

        // Destroy the block
        factory->destroy(blockPtr);
        blockPtr = nullptr;

        // Decrease the counter of the factory
        factory->removeRef();
        if (factory->getReferenceCount() == 1) {
            // Delete the memory ownership of the shared pointer used in this function
            factory.reset();

            // Delete the factory object
            if (!blockfactory::core::ClassFactorySingleton::getInstance().destroyFactory(
                    {"ExampleToolbox", "SignalMath"})) {
                bfError << "Failed to destroy the factory";

                // Do not return since other actions need to be performed
            }
        }

        // Delete the BlockInformation object
        delete blockInfo;
        blockInfo = nullptr;

        // Report errors
        if (!ok) {
            std::string error = blockfactory::core::Log::getSingleton().getErrors();
            error = "[Terminate]" + error;

            // Trim the message if needed
            if (error.length() >= 1024) {
                error = error.substr(0, 1024 - 1);
            }

            // This shouldn't happen
            if (getRTM()->errorStatus) {
                delete getRTM()->errorStatus;
                getRTM()->errorStatus = nullptr;
            }

            getRTM()->errorStatus = new char[1024];
            sprintf(const_cast<char_T*>(getRTM()->errorStatus), "%s", error.c_str());
            return;
        }
    }

    // End of S-Function Block: <Root>/Signal Math
}

/* Constructor */
AutogenerationExampleModelClass::AutogenerationExampleModelClass()
{
    static const P_AutogenerationExample_T AutogenerationExample_P_temp = {
        /* Expression: [1 2 3 4 5]
         * Referenced by: '<Root>/Constant'
         */
        {1.0, 2.0, 3.0, 4.0, 5.0}}; /* Modifiable parameters */

    /* Initialize tunable parameters */
    AutogenerationExample_P = AutogenerationExample_P_temp;
}

/* Destructor */
AutogenerationExampleModelClass::~AutogenerationExampleModelClass()
{
    /* Currently there is no destructor body generated.*/
}

/* Real-Time Model get method */
RT_MODEL_AutogenerationExampl_T* AutogenerationExampleModelClass::getRTM()
{
    return (&AutogenerationExample_M);
}
