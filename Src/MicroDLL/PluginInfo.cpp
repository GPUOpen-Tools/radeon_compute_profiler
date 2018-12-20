//==============================================================================
// Copyright (c) 2015 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  This file contains helper functions for the MicroDLL plugin.
//==============================================================================

#include "PluginInfo.h"
#include <vector>
#include <direct.h>
#include "..\Common\Defs.h"
#include "..\Common\StringUtils.h"
#include "..\Common\FileUtils.h"
#include "..\Common\Logger.h"
#include "DLLMain.h"
#include <tchar.h>

#include <AMDTOSWrappers/Include/osFile.h>
#include <AMDTOSWrappers/Include/osFilePath.h>

using std::vector;
using std::string;

static vector<PluginInfo> g_sPluginList;

void InitAvailablePluginInfo()
{
    Parameters params;
    FileUtils::GetParametersFromFile(params);

    SP_TODO("Solve Issues for installed libraries in unicode directory");
    // The converted path is used since detours is currently only installed in ascii folder:
    std::string convertedDllPath;
    StringUtils::WideStringToUtf8String(params.m_strDLLPath.asCharArray(), convertedDllPath);

    g_strOutputFile = params.m_strOutputFile;
    g_strDLLPath = convertedDllPath;
    g_strCounterFile = params.m_strCounterFile;

    // Add Micro.dll directory to dll search path
    // TODO: Refer to GPUPerfstudio changelist 361757
    SetDllDirectoryA(g_strDLLPath.c_str());

    g_strMicroDllPath = g_strDLLPath + MICRO_DLL;

    PluginInfo piCLOcc;
    piCLOcc.strPluginPath = g_strDLLPath + CL_OCCUPANCY_AGENT_DLL;
    piCLOcc.strPluginName = "OpenCL Occupancy Agent";
    piCLOcc.shouldForceLoad = false;

    PluginInfo piCLTrace;
    piCLTrace.strPluginPath = g_strDLLPath + CL_TRACE_AGENT_DLL;
    piCLTrace.strPluginName = "OpenCL Trace Agent";
    piCLTrace.shouldForceLoad = false;

    g_sPluginList.push_back(piCLTrace);
    g_sPluginList.push_back(piCLOcc);

    for (vector<PluginInfo>::iterator it = g_sPluginList.begin(); it != g_sPluginList.end(); it++)
    {
        if (!it->IsLoaded() && it->shouldForceLoad)
        {
            it->hModule = LoadLibraryA(it->strPluginPath.c_str());
        }
    }
}

void CheckOnLoadLibrary()
{
    for (vector<PluginInfo>::iterator it = g_sPluginList.begin(); it != g_sPluginList.end(); it++)
    {
        if (it->IsLoaded())
        {
            CallFunction(it->hModule, "UpdateHooks");
        }
        else
        {
            it->hModule = GetModuleHandleA(it->strPluginPath.c_str());
        }
    }
}

void CheckOnFreeLibrary()
{
    for (vector<PluginInfo>::iterator it = g_sPluginList.begin(); it != g_sPluginList.end(); it++)
    {
        if (it->IsLoaded() && NULL == GetModuleHandleA(it->strPluginPath.c_str()))
        {
            it->hModule = NULL;
        }
    }
}


bool CallFunction(HMODULE hLib, LPCSTR functionName)
{
    UpdateHooks_type pUpdateHooksFunc = (UpdateHooks_type)GetProcAddress(hLib, functionName);

    if (pUpdateHooksFunc != NULL)
    {
        return pUpdateHooksFunc();
    }
    else
    {
        return false;
    }
}

void NotifyOnExitProcess()
{
    for (vector<PluginInfo>::iterator it = g_sPluginList.begin(); it != g_sPluginList.end(); it++)
    {
        if (it->IsLoaded())
        {
            CallFunction(it->hModule, "OnExitProcess");
        }
    }
}
