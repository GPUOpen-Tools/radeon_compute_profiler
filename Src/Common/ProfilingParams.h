//==============================================================================
// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief This file contains the profiling parameters structure
//==============================================================================

#ifndef _PROFILING_PARAMS_H_
#define _PROFILING_PARAMS_H_

#include "Defs.h"

struct Parameters
{
    /// Default Constructor
    Parameters()
    {
        m_strOutputFile.clear();
        m_strSessionName.clear();
        m_strDLLPath = L"";
        m_strCounterFile.clear();
        m_strKernelFile.clear();
        m_strCmdArgs = L"";
        m_strWorkingDir = L"";
        m_strTimerDLLFile.clear();
        m_strUserTimerFn.clear();
        m_strUserTimerInitFn.clear();
        m_strUserTimerDestroyFn.clear();
        m_strUserPMCLibPath.clear();
        m_bVerbose = false;
        m_bOutputIL = false;
        m_bOutputISA = false;
        m_bOutputCL = false;
        m_bOutputHSAIL = false;
        m_bPerfCounter = false;
        m_bTrace = false;
        m_bHSATrace = false;
        m_bHSAPMC = false;
        m_bTimeOutBasedOutput = true;
        m_uiTimeOutInterval = DEFAULT_TIMEOUT_INTERVAL;
        m_bFullEnvBlock = false;
        m_cOutputSeparator = '\0';
        m_bTestMode = false;
        m_bUserTimer = false;
        m_bQueryRetStat = false;
        m_bCollapseClGetEventInfo = true;
        m_bStackTrace = false;
        m_bKernelOccupancy = false;
        m_uiVersionMajor = RCP_MAJOR_VERSION;
        m_uiVersionMinor = RCP_MINOR_VERSION;
        m_uiMaxNumOfAPICalls = DEFAULT_MAX_NUM_OF_API_CALLS;
        m_uiMaxKernels = DEFAULT_MAX_KERNELS;
        m_bUserPMC = false;
        m_bCompatibilityMode = false;
        m_bGMTrace = false;
        m_bForceSinglePassPMC = false;
        m_bGPUTimePMC = false;
        m_bStartDisabled = false;
        m_bDelayStartEnabled = false;
        m_bProfilerDurationEnabled = false;
        m_delayInMilliseconds = 0;
        m_durationInMilliseconds = 0;
        m_bForceSingleGPU = false;
        m_uiForcedGpuIndex = 0;
        m_bAqlPacketTracing = false;
        m_bDisableKernelDemangling = false;
    }

    unsigned int m_uiVersionMajor;                ///< Version major
    unsigned int m_uiVersionMinor;                ///< Version minor
    gtString m_strCmdArgs;                        ///< command line arguments
    gtString m_strWorkingDir;                     ///< working directory
    std::string m_strOutputFile;                  ///< output file path string
    std::string m_strSessionName;                 ///< session name string
    gtString m_strDLLPath;                        ///< GPUPerfAPIDLL path
    std::string m_strCounterFile;                 ///< Counter file
    std::string m_strKernelFile;                  ///< Kernel list file
    std::string m_strAPIFilterFile;               ///< API Filter file
    std::string m_strTimerDLLFile;                ///< User timer DLL file (including path)
    std::string m_strUserTimerFn;                 ///< User timer function name
    std::string m_strUserTimerInitFn;             ///< User timer initialization function (name)
    std::string m_strUserTimerDestroyFn;          ///< User timer destroy function (name)
    std::string m_strUserPMCLibPath;              ///< User PMC sampler module path
    bool m_bVerbose;                              ///< verbose option
    bool m_bOutputIL;                             ///< output OpenCL kernel IL file option
    bool m_bOutputHSAIL;                          ///< output Kernel HSAIL file option
    bool m_bOutputISA;                            ///< output OpenCL kernel ISA file option
    bool m_bOutputCL;                             ///< output OpenCL kernel CL file option
    bool m_bPerfCounter;                          ///< enable CL performance counter mode option
    bool m_bTrace;                                ///< enable CL trace mode option
    bool m_bTimeOutBasedOutput;                   ///< timeOut-based output model
    bool m_bHSATrace;                             ///< enable HSA trace mode option
    bool m_bHSAPMC;                               ///< enable HSA performance counter mode option
    unsigned int m_uiTimeOutInterval;             ///< Timeout interval
    EnvVarMap m_mapEnvVars;                       ///< an environment block for the profiled app
    bool m_bFullEnvBlock;                         ///< flag indicating whether or not the strEnvBlock represents a full environment block
    char m_cOutputSeparator;                      ///< output file separator character
    bool m_bTestMode;                             ///< internal test mode flag
    bool m_bUserTimer;                            ///< internal mode to use the user timer rather than the default Win32 timers
    bool m_bQueryRetStat;                         ///< Always query cl function status
    bool m_bCollapseClGetEventInfo;               ///< Collapse consecutive, identical clGetEventInfo calls into a single call
    bool m_bStackTrace;                           ///< Stack trace
    bool m_bKernelOccupancy;                      ///< Flag to signal whether to record kernel occupancy
    bool m_bUserPMC;                              ///< flag indicating whether or not user PMC sampler callbacks are invoked during CPU timestamp read.
    bool m_bCompatibilityMode;                    ///< flag indicating whether or not compatibility mode is enabled
    unsigned int m_uiMaxNumOfAPICalls;            ///< Maximum number of API calls
    unsigned int m_uiMaxKernels;                  ///< maximum number of kernels to profile.
    bool m_bGMTrace;                              ///< Flag indicating whether or not global memory trace is enabled
    bool m_bForceSinglePassPMC;                   ///< Flag indicating that only a single pass should be allowed when collecting performance counters
    bool m_bGPUTimePMC;                           ///< Flag indicating whether or not the profiler should collect gpu time when collecting perf counters
    bool m_bStartDisabled;                        ///< Flag indicating whether or not to start with profiling disabled
    KernelFilterList m_kernelFilterList;          ///< List of kernels to filter for perf counter profiling and subkernel profiling
    bool m_bDelayStartEnabled;                    ///< flag indicating whether to start profiler with a delay or not
    bool m_bProfilerDurationEnabled;              ///< flag indicating whether profiler should only run for certain duration
    unsigned int m_delayInMilliseconds;           ///< delay for profiler in milliseconds
    unsigned int m_durationInMilliseconds;        ///< duration for profiler in milliseconds for which profiler should run
    bool m_bForceSingleGPU;                       ///< Flag indicating whether or not to force a single GPU
    unsigned int m_uiForcedGpuIndex;              ///< Forced GPU index
    bool m_bAqlPacketTracing;                     ///< Flag indicating that an AQL Packet Trace should be performed.
    bool m_bDisableKernelDemangling;              ///< Flag indicating whether or not to demangle the kernel name
    bool m_bNoHSATransferTime;                    ///< Flag indicating whether or not HSA transfer time is ignored
};

#endif // _PROFILING_PARAMS_H_
