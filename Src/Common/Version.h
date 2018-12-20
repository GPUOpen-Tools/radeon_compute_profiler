//==============================================================================
// Copyright (c) 2015 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief This file contains the version number for rcprof.
//==============================================================================

#ifndef _VERSION_H_
#define _VERSION_H_

/// \defgroup Backend Backend
/// This is rcprof.
/// It handles code injection to the application process,
/// gathering GPU performance counters through GPUPerfAPI, collecting
/// statistics from CL and ROCm run-times.

/// \defgroup rcprof rcprof
/// This is the console application for the backend.
/// It handles command line inputs from the user or client and use Detours
/// and virtual table patching to inject into user application
///
/// \ingroup Backend
// @{

#define RCP_MAJOR_VERSION 5
#define RCP_MINOR_VERSION 6
#define RCP_BUILD_NUMBER 0
#define RCP_UPDATE_VERSION 0

#define RCP_STR_VALUE(s) #s
#define RCP_VERSION_STR(s) RCP_STR_VALUE(s)
#define RCP_JOIN_STRINGS(ARG1,ARG2) RCP_STR_VALUE(ARG1) RCP_STR_VALUE(ARG2)

#define RCP_VERSION RCP_MAJOR_VERSION.RCP_MINOR_VERSION.RCP_BUILD_NUMBER

#define RCP_VERSION_STRING RCP_JOIN_STRINGS(V,RCP_VERSION)

#define RCP_MAJOR_VERSION_STR RCP_VERSION_STR(RCP_MAJOR_VERSION)
#define RCP_MINOR_VERSION_STR RCP_VERSION_STR(RCP_MINOR_VERSION)
#define RCP_BUILD_NUMBER_STR RCP_VERSION_STR(RCP_BUILD_NUMBER)
#define RCP_UPDATE_VERSION_STR RCP_VERSION_STR(RCP_UPDATE_VERSION)

#define AMDT_PROJECT_SUFFIX_STR RCP_VERSION_STR(AMDT_PROJECT_SUFFIX)

#define RCP_EXE_BASE_NAME "rcprof"
#define RCP_EXE_NAME_STR RCP_EXE_BASE_NAME AMDT_PROJECT_SUFFIX_STR
#define GPU_PROFILER_LIBRARY_NAME_PREFIX "RCP"
#define RCP_PRODUCT_NAME "Radeon Compute Profiler"

#define RCP_COPYRIGHT_CURRENT_YEAR 2018

#define RCP_COPYRIGHT_STR "Copyright (c) 2010-" RCP_VERSION_STR(RCP_COPYRIGHT_CURRENT_YEAR) " Advanced Micro Devices, Inc. All rights reserved."

// @}

#endif // _VERSION_H_
