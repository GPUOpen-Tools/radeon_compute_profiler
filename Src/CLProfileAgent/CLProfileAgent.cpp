//==============================================================================
// Copyright (c) 2015-2018 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief A collection of functions to handle the interaction with
///        the OpenCL Profiling Agent API.
//==============================================================================

#include <string.h>
#include <iostream>
#include "CLFunctionDefs.h"
#include "CLProfileAgent.h"
#include "CLProfilerMineCLEntry.h"
#include "CLGPAProfiler.h"
#include "../Common/Logger.h"
#include "../Common/Defs.h"
#include "../Common/Version.h"
#include "../Common/FileUtils.h"
#include "../Common/GlobalSettings.h"

static cl_icd_dispatch_table original_dispatch;
static cl_icd_dispatch_table modified_dispatch;

extern CLGPAProfiler g_Profiler;

extern "C" DLL_PUBLIC void amdtCodeXLStopProfiling()
{
    g_Profiler.EnableProfiling(false);
}

extern "C" DLL_PUBLIC void amdtCodeXLResumeProfiling()
{
    g_Profiler.EnableProfiling(true);
}

cl_int CL_CALLBACK
clAgent_OnLoad(cl_agent* agent)
{
#ifdef _DEBUG
    FileUtils::CheckForDebuggerAttach();
#endif

    std::string strLogFile = FileUtils::GetDefaultOutputPath() + "clprofileagent.log";
    GPULogger::LogFileInitialize(strLogFile.c_str());

    cl_icd_dispatch_table nextTable;
    cl_icd_dispatch_table realTable;
    cl_int status = InitAgent(agent, CL_PROFILE_AGENT_DLL, &nextTable, &realTable);

    if (CL_SUCCESS != status)
    {
        return CL_SUCCESS;
    }

    std::cout << RCP_PRODUCT_NAME " " << RCP_VERSION_STRING << " is enabled\n";

    memcpy(&original_dispatch, &nextTable, sizeof(original_dispatch));
    memcpy(&modified_dispatch, &original_dispatch, sizeof(modified_dispatch));

    InitNextCLFunctions(&nextTable, &realTable);
    CreateMineDispatchTable(modified_dispatch);

    status = agent->SetICDDispatchTable(agent, &modified_dispatch, sizeof(modified_dispatch));

    // Pass params between processes through file
    // rcprof generates a text file in current dir
    Parameters params;
    FileUtils::GetParametersFromFile(params);
    FileUtils::ReadKernelListFile(params);

    GlobalSettings::GetInstance()->m_bVerbose = params.m_bVerbose;
    GlobalSettings::GetInstance()->m_params = params;

    if (status != CL_SUCCESS)
    {
        return status;
    }

    return CL_SUCCESS;
}
