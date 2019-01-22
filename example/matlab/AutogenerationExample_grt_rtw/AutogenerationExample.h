/*
 * AutogenerationExample.h
 *
 * Non-Degree Granting Education License -- for use at non-degree
 * granting, nonprofit, educational organizations only. Not for
 * government, commercial, or other organizational use.
 *
 * Code generation for model "AutogenerationExample".
 *
 * Model version              : 1.7
 * Simulink Coder version : 8.14 (R2018a) 06-Feb-2018
 * C++ source code generated on : Fri Dec 14 16:49:39 2018
 *
 * Target selection: grt.tlc
 * Note: GRT includes extra infrastructure and instrumentation for prototyping
 * Embedded hardware selection: Intel->x86-64 (Windows64)
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#ifndef RTW_HEADER_AutogenerationExample_h_
#define RTW_HEADER_AutogenerationExample_h_
#include <stddef.h>
#include <string.h>
#ifndef AutogenerationExample_COMMON_INCLUDES_
#define AutogenerationExample_COMMON_INCLUDES_
#include "rtw_continuous.h"
#include "rtw_solver.h"
#include "rtwtypes.h"
#include <BlockFactory/Core/Block.h>
#include <BlockFactory/Core/FactorySingleton.h>
#include <BlockFactory/Core/Log.h>
#include <BlockFactory/Core/Parameter.h>
#include <BlockFactory/Core/Parameters.h>
#include <BlockFactory/SimulinkCoder/CoderBlockInformation.h>
#include <cstdio>
#endif /* AutogenerationExample_COMMON_INCLUDES_ */

#include "AutogenerationExample_types.h"

/* Shared type includes */
#include "multiword_types.h"

/* Macros for accessing real-time model data structure */
#ifndef rtmGetErrorStatus
#define rtmGetErrorStatus(rtm) ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
#define rtmSetErrorStatus(rtm, val) ((rtm)->errorStatus = (val))
#endif

/* Block states (default storage) for system '<Root>' */
typedef struct
{
    struct
    {
        void* blockPWork[2];
    } SignalMath_PWORK; /* '<Root>/Signal Math' */
} DW_AutogenerationExample_T;

/* External inputs (root inport signals with default storage) */
typedef struct
{
    real_T input2[5]; /* '<Root>/Input2' */
} ExtU_AutogenerationExample_T;

/* External outputs (root outports fed by signals with default storage) */
typedef struct
{
    real_T Result[5]; /* '<Root>/Result' */
} ExtY_AutogenerationExample_T;

/* Parameters (default storage) */
struct P_AutogenerationExample_T_
{
    real_T Constant_Value[5]; /* Expression: [1 2 3 4 5]
                               * Referenced by: '<Root>/Constant'
                               */
};

/* Real-time Model Data Structure */
struct tag_RTM_AutogenerationExample_T
{
    const char_T* errorStatus;
};

/* Class declaration for model AutogenerationExample */
class AutogenerationExampleModelClass
{
    /* public data and function members */
public:
    /* External inputs */
    ExtU_AutogenerationExample_T AutogenerationExample_U;

    /* External outputs */
    ExtY_AutogenerationExample_T AutogenerationExample_Y;

    /* model initialize function */
    void initialize();

    /* model step function */
    void step();

    /* model terminate function */
    void terminate();

    /* Constructor */
    AutogenerationExampleModelClass();

    /* Destructor */
    ~AutogenerationExampleModelClass();

    /* Real-Time Model get method */
    RT_MODEL_AutogenerationExampl_T* getRTM();

    /* private data and function members */
private:
    /* Tunable parameters */
    P_AutogenerationExample_T AutogenerationExample_P;

    /* Block states */
    DW_AutogenerationExample_T AutogenerationExample_DW;

    /* Real-Time Model */
    RT_MODEL_AutogenerationExampl_T AutogenerationExample_M;
};

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Use the MATLAB hilite_system command to trace the generated code back
 * to the model.  For example,
 *
 * hilite_system('<S3>')    - opens system 3
 * hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'AutogenerationExample'
 */
#endif /* RTW_HEADER_AutogenerationExample_h_ */
