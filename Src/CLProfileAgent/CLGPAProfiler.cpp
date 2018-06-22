//==============================================================================
// Copyright (c) 2015-2018 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief The implementation for the GPA helper class.
//==============================================================================

// std
#include <set>
#include <stdio.h>
#include <iostream>

// common
#include <AMDTOSWrappers/Include/osFilePath.h>
#include <AMDTOSWrappers/Include/osProcess.h>

// profiler common
#include <SeqIDGenerator.h>
#include <StringUtils.h>
#include <FileUtils.h>
#include <GlobalSettings.h>
#include <Version.h>
#include <ProfilerOutputFileDefs.h>
#include <Logger.h>

// CL common
#include <CLFunctionDefs.h>
#include <CLUtils.h>
#include <CLPlatformInfo.h>

#include "CLGPAProfiler.h"

using namespace std;
using namespace CLUtils;
using namespace GPULogger;

extern CLGPAProfiler g_Profiler;

CLGPAProfiler::CLGPAProfiler() :
    m_isGPAOpened(false),
    m_uiCurKernelCount(0),
    m_uiMaxKernelCount(DEFAULT_MAX_KERNELS),
    m_uiOutputLineCount(0),
    m_bIsProfilingEnabled(true),
    m_bGPU(false),
    m_bDelayStartEnabled(false),
    m_bProfilerDurationEnabled(false),
    m_delayInMilliseconds(0ul),
    m_durationInMilliseconds(0ul),
    m_pDelayTimer(nullptr),
    m_pDurationTimer(nullptr)
{
    m_strOutputFile.clear();
}


void CLGPAProfilerTimerEndResponse(ProfilerTimerType timerType)
{
    switch (timerType)
    {
        case PROFILEDELAYTIMER:
            g_Profiler.EnableProfiling(true);
            unsigned long profilerDuration;

            if (g_Profiler.IsProfilerDurationEnabled(profilerDuration))
            {
                g_Profiler.CreateTimer(PROFILEDURATIONTIMER, profilerDuration);
                g_Profiler.SetTimerFinishHandler(PROFILEDURATIONTIMER, CLGPAProfilerTimerEndResponse);
                g_Profiler.StartTimer(PROFILEDURATIONTIMER);
            }

            break;

        case PROFILEDURATIONTIMER:
            g_Profiler.EnableProfiling(false);
            break;

        default:
            break;
    }
}

CLGPAProfiler::~CLGPAProfiler()
{
    // delete unreferenced user event, deferred kernel
    // If a program is well-written, both list should be empty
    for (UserEventList::iterator it = m_userEventList.begin(); it != m_userEventList.end(); it++)
    {
        delete(*it);
    }

    if (nullptr != m_pDelayTimer)
    {
        m_pDelayTimer->stopTimer();
        delete m_pDelayTimer;
    }

    if (nullptr != m_pDurationTimer)
    {
        m_pDurationTimer->stopTimer();
        delete m_pDurationTimer;
    }
}

bool CLGPAProfiler::Open(cl_command_queue commandQueue)
{
    SpAssertRet(!m_isGPAOpened) false;

    SeqIDGenerator::Instance()->EnableGenerator(false);
    m_isGPAOpened = m_gpaUtils.Open((void*)commandQueue);
    return m_isGPAOpened;
}

bool CLGPAProfiler::Close()
{
    bool retVal = true;
    SeqIDGenerator::Instance()->EnableGenerator(true);

    if (m_isGPAOpened)
    {
        retVal = m_gpaUtils.Close();
        m_isGPAOpened = !retVal;
    }

    return retVal;
}

bool CLGPAProfiler::AddContext(const cl_context context)
{
    std::string strError;

    // if we haven't already initialized the profiler, this is a good time to do so
    if (!m_gpaUtils.Loaded())
    {
        if (!g_Profiler.Init(GlobalSettings::GetInstance()->m_params, strError))
        {
            // if we are unable to intialize the GPA, output an error
#ifdef USE_DEBUG_GPA
            static std::string strDll(LIB_PREFIX "GPUPerfAPICL" BITNESS "-d" AMDT_BUILD_SUFFIX LIB_SUFFIX);
#else
            static std::string strDll(LIB_PREFIX "GPUPerfAPICL" BITNESS AMDT_BUILD_SUFFIX LIB_SUFFIX);
#endif
            std::cout << "Error loading " << strDll << ": " << strError << std::endl;
            GPULogger::Log(GPULogger::logERROR, "Error loading %s. Error: %s\n", strDll.c_str(), strError.c_str());
        }
    }

    return m_contextManager.AddContext(context);
}

bool CLGPAProfiler::RemoveContext(const cl_context context)
{
    return m_contextManager.RemoveContext(context);
}

bool CLGPAProfiler::AddKernel(const cl_kernel kernel)
{
    cl_context context;
    cl_int status = g_realDispatchTable.GetKernelInfo(kernel, CL_KERNEL_CONTEXT, sizeof(cl_context), &context, NULL);

    if (CL_SUCCESS != status)
    {
        return false;
    }

    return m_contextManager.AddKernelToContext(context, kernel);
}

bool CLGPAProfiler::RemoveMemObject(const cl_mem& mem)
{
    cl_context ctx;
    cl_int nRet = g_realDispatchTable.GetMemObjectInfo(mem, CL_MEM_CONTEXT, sizeof(cl_context), &ctx, NULL);

    if (nRet != CL_SUCCESS)
    {
        return false;
    }

    return m_contextManager.RemoveBuffer(ctx, mem);
}

bool CLGPAProfiler::RemoveKernel(const cl_kernel kernel)
{
    cl_context context;
    cl_int status = g_realDispatchTable.GetKernelInfo(kernel, CL_KERNEL_CONTEXT, sizeof(cl_context), &context, NULL);

    if (CL_SUCCESS != status)
    {
        return false;
    }

    return m_contextManager.RemoveKernelFromContext(context, kernel);
}

bool CLGPAProfiler::AddBuffer(const cl_context& context,
                              const cl_mem&      buffer,
                              cl_mem_flags      flags,
                              size_t            bufferSize,
                              void*             pHost)
{
    return m_contextManager.AddBufferToContext(context, buffer, flags, bufferSize, pHost);
}

bool CLGPAProfiler::AddSubBuffer(const cl_mem& parentBuffer,
                                 const cl_mem& subBuffer,
                                 cl_mem_flags  flags,
                                 size_t        bufferSize)
{
    return m_contextManager.AddSubBuffer(parentBuffer, subBuffer, flags, bufferSize);
}

bool CLGPAProfiler::AddPipe(const cl_context context, const cl_mem pipe)
{
    return m_contextManager.AddPipeToContext(context, pipe);
}

bool CLGPAProfiler::AddKernelArg(const cl_kernel kernel, cl_uint argIdx, const void* pArgValue)
{
    cl_context context;
    cl_int status = g_realDispatchTable.GetKernelInfo(kernel, CL_KERNEL_CONTEXT, sizeof(cl_context), &context, NULL);

    if (CL_SUCCESS != status)
    {
        return false;
    }

    return m_contextManager.AddKernelArg(context, kernel, argIdx, pArgValue);
}

bool CLGPAProfiler::AddKernelArgSVMPointer(const cl_kernel kernel, cl_uint argIdx)
{
    bool retVal = false;

    cl_context context;
    cl_int status = g_realDispatchTable.GetKernelInfo(kernel, CL_KERNEL_CONTEXT, sizeof(cl_context), &context, NULL);

    if (CL_SUCCESS == status)
    {
        retVal = m_contextManager.AddKernelArgSVMPointer(context, kernel, argIdx);
    }

    return retVal;
}

bool CLGPAProfiler::HasKernelArgSVMPointer(const cl_kernel kernel)
{
    bool retVal = false;

    cl_context context;
    cl_int status = g_realDispatchTable.GetKernelInfo(kernel, CL_KERNEL_CONTEXT, sizeof(cl_context), &context, NULL);

    if (CL_SUCCESS == status)
    {
        retVal = m_contextManager.HasKernelArgSVMPointer(context, kernel);
    }

    return retVal;
}

bool CLGPAProfiler::AddKernelArgPipe(const cl_kernel kernel, cl_uint argIdx)
{
    bool retVal = false;

    cl_context context;
    cl_int status = g_realDispatchTable.GetKernelInfo(kernel, CL_KERNEL_CONTEXT, sizeof(cl_context), &context, NULL);

    if (CL_SUCCESS == status)
    {
        retVal = m_contextManager.AddKernelArgPipe(context, kernel, argIdx);
    }

    return retVal;
}

bool CLGPAProfiler::HasKernelArgPipe(const cl_kernel kernel)
{
    bool retVal = false;

    cl_context context;
    cl_int status = g_realDispatchTable.GetKernelInfo(kernel, CL_KERNEL_CONTEXT, sizeof(cl_context), &context, NULL);

    if (CL_SUCCESS == status)
    {
        retVal = m_contextManager.HasKernelArgPipe(context, kernel);
    }

    return retVal;
}

bool CLGPAProfiler::Init(const Parameters& params, string& strErrorOut)
{
    // Set kernel files
    m_KernelAssembly.SetOutputIL(params.m_bOutputIL);
    m_KernelAssembly.SetOutputISA(params.m_bOutputISA);
    m_KernelAssembly.SetOutputCL(params.m_bOutputCL);
    m_KernelAssembly.SetOutputHSAIL(params.m_bOutputHSAIL);

    // Set output file
    SetOutputFile(params.m_strOutputFile);

    // Set list separator
    KernelProfileResultManager::Instance()->SetListSeparator(params.m_cOutputSeparator);

    // Get platform info
    if (!CLUtils::GetPlatformInfo(m_platformList))
    {
        m_platformList.clear();
    }

    m_uiMaxKernelCount = params.m_uiMaxKernels;
    m_bForceSinglePass = params.m_bForceSinglePassPMC;
    m_bCollectGPUTime = m_bForceSinglePass ? false : params.m_bGPUTimePMC;
    size_t nMaxPass = m_bForceSinglePass ? 1 : GPA_INFINITE_PASS;

    if (!params.m_bStartDisabled)
    {
        m_bDelayStartEnabled = params.m_bDelayStartEnabled;
        m_bProfilerDurationEnabled = params.m_bProfilerDurationEnabled;
        m_delayInMilliseconds = params.m_delayInMilliseconds;
        m_durationInMilliseconds = params.m_durationInMilliseconds;
        m_bIsProfilingEnabled = m_delayInMilliseconds > 0 ? false : true;

        if (m_bDelayStartEnabled)
        {
            CreateTimer(PROFILEDELAYTIMER, m_delayInMilliseconds);

            if (nullptr != m_pDelayTimer)
            {
                m_pDelayTimer->SetTimerFinishHandler(CLGPAProfilerTimerEndResponse);
                m_pDelayTimer->startTimer(true);
            }
        }
        else if (m_bProfilerDurationEnabled)
        {
            CreateTimer(PROFILEDURATIONTIMER, m_durationInMilliseconds);

            if (nullptr != m_pDurationTimer)
            {
                m_pDurationTimer->SetTimerFinishHandler(CLGPAProfilerTimerEndResponse);
                m_pDurationTimer->startTimer(true);
            }
        }
    }
    else
    {
        m_bIsProfilingEnabled = !params.m_bStartDisabled;
    }


    gtString strDllPath = params.m_strDLLPath;

    if (strDllPath.isEmpty())
    {
        gtString strAgentPath;
        osGetCurrentProcessEnvVariableValue(L"CL_AGENT", strAgentPath);

        if (!strAgentPath.isEmpty())
        {
            osFilePath agentPath(strAgentPath);
            strDllPath = agentPath.fileDirectoryAsString();
            strDllPath.append(osFilePath::osPathSeparator);
        }
    }

    CounterList enabledCounters;
    m_gpaUtils.InitGPA(GPA_API_OPENCL,
                       strDllPath,
                       strErrorOut,
                       params.m_strCounterFile.empty() ? NULL : params.m_strCounterFile.c_str(),
                       &enabledCounters,
                       nMaxPass);

    // Enable all counters if no counter file is specified or counter file is empty.
    if (enabledCounters.empty())
    {
#ifdef AMDT_INTERNAL
        // Internal mode must have a counter file specified.
        std::cout << "Please specify a counter file using -c. No counter is enabled." << endl;
        return false;
#else

        // get the device ids for all installed adapters
        AsicInfoList asicInfoList;
        AMDTADLUtils::Instance()->GetAsicInfoList(asicInfoList);

        set<string> counterSet;
        vector<string> tempCounters;

        // Enable all counters
        for (CLPlatformSet::iterator idxPlatform = m_platformList.begin(); idxPlatform != m_platformList.end(); idxPlatform++)
        {
            GDT_HW_GENERATION gen;
            vector<string> counterNames;

            if (0 != idxPlatform->m_pcieDeviceId)
            {
                m_gpaUtils.GetAvailableCountersForDevice(idxPlatform->m_pcieDeviceId, REVISION_ID_ANY, nMaxPass, counterNames);
            }

            if (counterNames.empty())
            {
                string translatedDeviceName = AMDTDeviceInfoUtils::Instance()->TranslateDeviceName(idxPlatform->m_deviceName.c_str());

                int deviceId;
                int revisionId;
                bool retVal = GetAvailableDeviceIdFromDeviceNameAndAsicInfoList(translatedDeviceName.c_str(), asicInfoList, deviceId, revisionId);

                if (retVal)
                {
                    m_gpaUtils.GetAvailableCountersForDevice(deviceId, revisionId, nMaxPass, counterNames);
                }

                if (counterNames.empty())
                {
                    bool isGenSet = false;

                    isGenSet = AMDTDeviceInfoUtils::Instance()->GetHardwareGeneration(static_cast<size_t>(deviceId), gen);

                    if (!isGenSet)
                    {
                        isGenSet = AMDTDeviceInfoUtils::Instance()->GetHardwareGeneration(translatedDeviceName.c_str(), gen);
                    }

                    if (isGenSet)
                    {
                        switch (gen)
                        {
                            case GDT_HW_GENERATION_SOUTHERNISLAND:
                            {
                                m_gpaUtils.GetAvailableCounters(GPA_HW_GENERATION_SOUTHERNISLAND, counterNames);
                                break;
                            }

                            case GDT_HW_GENERATION_SEAISLAND:
                            {
                                m_gpaUtils.GetAvailableCounters(GPA_HW_GENERATION_SEAISLAND, counterNames);
                                break;
                            }

                            case GDT_HW_GENERATION_VOLCANICISLAND:
                            {
                                m_gpaUtils.GetAvailableCounters(GPA_HW_GENERATION_VOLCANICISLAND, counterNames);
                                break;
                            }

                            default:
                                break;
                        }
                    }
                }
            }

            tempCounters.insert(tempCounters.end(), counterNames.begin(), counterNames.end());
        }

        // remove duplicated counter
        for (vector<string>::iterator it = tempCounters.begin(); it != tempCounters.end(); ++it)
        {
            if (counterSet.find(*it) == counterSet.end())
            {
                counterSet.insert(*it);
                enabledCounters.push_back(*it);
            }
        }

        m_gpaUtils.SetEnabledCounters(enabledCounters);

#endif // AMDT_INTERNAL
    }

    // Init CSV file header and column row
    InitHeader();

    for (vector<string>::iterator it = enabledCounters.begin(); it != enabledCounters.end(); ++it)
    {
        KernelProfileResultManager::Instance()->AddProfileResultItem(*it);
    }

    return true;
}

bool CLGPAProfiler::FullProfile(
    cl_command_queue commandQueue,
    cl_kernel        kernel,
    cl_uint          uWorkDim,
    const size_t*    pGlobalWorkOffset,
    const size_t*    pGlobalWorkSize,
    const size_t*    pLocalWorkSize,
    cl_uint          uEventWaitList,
    const cl_event*  pEventWaitList,
    cl_event*        pEvent,
    cl_int&          nResultOut,
    GPA_SessionId&   sessionIdOut,
    double&          dKernelTimeOut)
{
    if (!m_gpaUtils.Loaded())
    {
        return false;
    }

    ++m_uiCurKernelCount;

    // get context from the command queue
    cl_context context = NULL;
    g_realDispatchTable.GetCommandQueueInfo(commandQueue, CL_QUEUE_CONTEXT, sizeof(cl_context), &context, NULL);

    bool kernelAlreadyDispatched = false;

    if (!m_bForceSinglePass)
    {
        // save related memory buffers (that are used for both read and write) before
        // we dispatch the kernel so we can load these buffers prior to starting
        // the next pass of the kernel
        // only need to save if kernel has r/w buffers and we are doing multi-pass
        m_contextManager.SaveArena(context, commandQueue, kernel);
    }

    if (m_bCollectGPUTime)
    {
        cl_event* pTmpEvent = pEvent;
        cl_event event1;

        if (pTmpEvent == NULL)
        {
            pTmpEvent = &event1;
        }

        // run the kernel once to workaround the high cost of the first kernel run.  NOTE: GPU Time is also collected for this run
        // call the entry in g_nextDispatchTable to pass the call down the agent chain to the runtime
        nResultOut = g_nextDispatchTable.EnqueueNDRangeKernel(commandQueue,
                                                              kernel,
                                                              uWorkDim,
                                                              pGlobalWorkOffset,
                                                              pGlobalWorkSize,
                                                              pLocalWorkSize,
                                                              uEventWaitList,
                                                              pEventWaitList,
                                                              pTmpEvent);

        if (CL_SUCCESS != nResultOut)
        {
            // there is a cl error, so we don't need to continue
            return false;
        }

        kernelAlreadyDispatched = true;

        // get the kernel time
        CLUtils::GetElapsedTimeFromEvent(pTmpEvent, dKernelTimeOut);

        if (NULL == pEvent)
        {
            // release the temporary event created otherwise buffer used won't get released
            g_realDispatchTable.ReleaseEvent(*pTmpEvent);
        }
    }
    else
    {
        dKernelTimeOut = 0;
    }

    if (m_isGPAOpened)
    {
        if (m_gpaUtils.CreateSession(sessionIdOut))
        {
            if (m_gpaUtils.EnableCounters(sessionIdOut) && GPA_STATUS_OK == m_gpaUtils.StatusCheck(m_gpaUtils.GetGPAFuncTable()->GPA_BeginSession(sessionIdOut)))
            {
                gpa_uint32 gpaPassCount = 0;
                m_gpaUtils.StatusCheck(m_gpaUtils.GetGPAFuncTable()->GPA_GetPassCount(sessionIdOut, &gpaPassCount));

                for (gpa_uint32 curPass = 0; curPass < gpaPassCount; curPass++)
                {
                    if (!m_bForceSinglePass && kernelAlreadyDispatched)
                    {
                        // load related memory buffers before starting the next kernel dispatch
                        // so that we can guarantee correctness of the multiple passes of the same kernel
                        m_contextManager.LoadArena(context, commandQueue, kernel);

                        if (NULL != pEvent)
                        {
                            // release the created event otherwise buffer used won't get released
                            g_realDispatchTable.ReleaseEvent(*pEvent);
                        }
                    }

                    GPA_CommandListId commandListId = nullptr;
                    m_gpaUtils.StatusCheck(m_gpaUtils.GetGPAFuncTable()->GPA_BeginCommandList(sessionIdOut, curPass, GPA_NULL_COMMAND_LIST, GPA_COMMAND_LIST_NONE, &commandListId));
                    m_gpaUtils.StatusCheck(m_gpaUtils.GetGPAFuncTable()->GPA_BeginSample(0, commandListId));

                    // call the entry in g_realDispatchTable to prevent other agents from seeing the replayed kernels
                    cl_int clRetVal = g_realDispatchTable.EnqueueNDRangeKernel(commandQueue,
                                                                               kernel,
                                                                               uWorkDim,
                                                                               pGlobalWorkOffset,
                                                                               pGlobalWorkSize,
                                                                               pLocalWorkSize,
                                                                               uEventWaitList,
                                                                               pEventWaitList,
                                                                               pEvent);

                    if (!kernelAlreadyDispatched)
                    {
                        nResultOut = clRetVal;
                    }

                    kernelAlreadyDispatched = true;

                    m_gpaUtils.StatusCheck(m_gpaUtils.GetGPAFuncTable()->GPA_EndSample(commandListId));
                    m_gpaUtils.StatusCheck(m_gpaUtils.GetGPAFuncTable()->GPA_EndCommandList(commandListId));

                    if (nResultOut != CL_SUCCESS)
                    {
                        break;
                    }

                    if (NULL != pEvent)
                    {
                        g_realDispatchTable.WaitForEvents(1, pEvent);
                    }
                }

                m_gpaUtils.StatusCheck(m_gpaUtils.GetGPAFuncTable()->GPA_EndSession(sessionIdOut));

                if (!m_bForceSinglePass)
                {
                    m_contextManager.ClearArena(context, kernel);
                }

                return true;
            }
        }
    }

    if (!kernelAlreadyDispatched)
    {
        // make sure we dispatch the kernel at least once see BUG450137
        nResultOut = g_nextDispatchTable.EnqueueNDRangeKernel(commandQueue,
                                                              kernel,
                                                              uWorkDim,
                                                              pGlobalWorkOffset,
                                                              pGlobalWorkSize,
                                                              pLocalWorkSize,
                                                              uEventWaitList,
                                                              pEventWaitList,
                                                              pEvent);
    }

    return false;
}

bool CLGPAProfiler::GenerateKernelAssembly(const cl_command_queue& commandQueue,
                                           const cl_kernel&        kernel,
                                           const std::string&      strKernelName,
                                           const std::string&      strKernelHandle)
{
    std::string strOutputDir;

    if (!FileUtils::GetWorkingDirectory(m_strOutputFile, strOutputDir))
    {
        return false;
    }

    return m_KernelAssembly.Generate(commandQueue,
                                     kernel,
                                     strKernelName,
                                     strKernelHandle,
                                     strOutputDir);

}

const KernelInfo& CLGPAProfiler::GetKernelInfoFromKernelAssembly(std::string& strKernelName) const
{
    return m_KernelAssembly.GetKernelInfo(strKernelName);
}

void CLGPAProfiler::InitHeader()
{
    KernelProfileResultManager::Instance()->AddHeader(StringUtils::FormatString("%s=%d.%d", FILE_HEADER_PROFILE_FILE_VERSION, RCP_MAJOR_VERSION, RCP_MINOR_VERSION));
    KernelProfileResultManager::Instance()->AddHeader(StringUtils::FormatString("%s=%d.%d.%d", FILE_HEADER_PROFILER_VERSION, RCP_MAJOR_VERSION, RCP_MINOR_VERSION, RCP_BUILD_NUMBER));
    KernelProfileResultManager::Instance()->AddHeader(StringUtils::FormatString("%s=OpenCL", FILE_HEADER_API));
    KernelProfileResultManager::Instance()->AddHeader(StringUtils::FormatString("%s=%s", FILE_HEADER_APPLICATION, FileUtils::GetExeFullPath().c_str()));
    KernelProfileResultManager::Instance()->AddHeader(StringUtils::FormatString("%s=%s", FILE_HEADER_APPLICATION_ARGS, GlobalSettings::GetInstance()->m_params.m_strCmdArgs.asUTF8CharArray()));
    KernelProfileResultManager::Instance()->AddHeader(StringUtils::FormatString("%s=%s", FILE_HEADER_WORKING_DIRECTORY, GlobalSettings::GetInstance()->m_params.m_strWorkingDir.asUTF8CharArray()));

    EnvVarMap envVarMap = GlobalSettings::GetInstance()->m_params.m_mapEnvVars;

    if (!envVarMap.empty())
    {
        KernelProfileResultManager::Instance()->AddHeader(StringUtils::FormatString("%s=%d", FILE_HEADER_FULL_ENVIRONMENT, GlobalSettings::GetInstance()->m_params.m_bFullEnvBlock));

        for (EnvVarMap::const_iterator it = envVarMap.begin(); it != envVarMap.end(); ++it)
        {
            KernelProfileResultManager::Instance()->AddHeader(StringUtils::FormatString("%s=%s=%s", FILE_HEADER_ENV_VAR, it->first.asUTF8CharArray(), it->second.asUTF8CharArray()));
        }
    }

    for (CLPlatformSet::iterator idxPlatform = m_platformList.begin(); idxPlatform != m_platformList.end(); idxPlatform++)
    {
        KernelProfileResultManager::Instance()->AddHeader(StringUtils::FormatString("Device %s Platform Vendor=%s", idxPlatform->m_deviceName.c_str(), idxPlatform->m_platformVendor.c_str()));
        KernelProfileResultManager::Instance()->AddHeader(StringUtils::FormatString("Device %s Platform Name=%s", idxPlatform->m_deviceName.c_str(), idxPlatform->m_platformName.c_str()));
        KernelProfileResultManager::Instance()->AddHeader(StringUtils::FormatString("Device %s Platform Version=%s", idxPlatform->m_deviceName.c_str(), idxPlatform->m_platformVersion.c_str()));
        KernelProfileResultManager::Instance()->AddHeader(StringUtils::FormatString("Device %s CLDriver Version=%s", idxPlatform->m_deviceName.c_str(), idxPlatform->m_driverVersion.c_str()));
        KernelProfileResultManager::Instance()->AddHeader(StringUtils::FormatString("Device %s CLRuntime Version=%s", idxPlatform->m_deviceName.c_str(), idxPlatform->m_runtimeVersion.c_str()));
        KernelProfileResultManager::Instance()->AddHeader(StringUtils::FormatString("Device %s NumberAppAddressBits=%d", idxPlatform->m_deviceName.c_str(), idxPlatform->m_addressBits));

        if (!idxPlatform->m_boardName.empty())
        {
            KernelProfileResultManager::Instance()->AddHeader(StringUtils::FormatString("Device %s Board Name=%s", idxPlatform->m_deviceName.c_str(), idxPlatform->m_boardName.c_str()));
        }

        if (0 != idxPlatform->m_pcieDeviceId)
        {
            KernelProfileResultManager::Instance()->AddHeader(StringUtils::FormatString("Device %s PCIE device id=%d", idxPlatform->m_deviceName.c_str(), idxPlatform->m_pcieDeviceId));
        }
    }

    KernelProfileResultManager::Instance()->AddHeader(StringUtils::FormatString("%s=%s", FILE_HEADER_OS_VERSION, OSUtils::Instance()->GetOSInfo().c_str()));
    KernelProfileResultManager::Instance()->AddHeader(StringUtils::FormatString("%s=%s", FILE_HEADER_DISPLAY_NAME, GlobalSettings::GetInstance()->m_params.m_strSessionName.c_str()));
    KernelProfileResultManager::Instance()->AddHeader(StringUtils::FormatString("%s=%c", FILE_HEADER_LIST_SEPARATOR, GlobalSettings::GetInstance()->m_params.m_cOutputSeparator));
    KernelProfileResultManager::Instance()->AddHeader(StringUtils::FormatString("%s=%s", FILE_HEADER_FORCE_SINGLE_PASS, GlobalSettings::GetInstance()->m_params.m_bForceSinglePassPMC ? "True" : "False"));

    if (GlobalSettings::GetInstance()->m_params.m_kernelFilterList.size() > 0)
    {
        std::string strKernelList;

        for (auto kernelName : GlobalSettings::GetInstance()->m_params.m_kernelFilterList)
        {
            if (!strKernelList.empty())
            {
                strKernelList.append(",");
            }

            strKernelList.append(kernelName);
        }

        KernelProfileResultManager::Instance()->AddHeader(StringUtils::FormatString("%s=%s", FILE_HEADER_KERNELS_PROFILED, strKernelList.c_str()));
    }
    else if (!GlobalSettings::GetInstance()->m_params.m_strKernelFile.empty())
    {
        KernelProfileResultManager::Instance()->AddHeader(StringUtils::FormatString("%s=A kernel list file %s was specified, but no kernels were read from it. All kernels have been profiled", FILE_HEADER_KERNELS_PROFILED, GlobalSettings::GetInstance()->m_params.m_strKernelFile.c_str()));
    }

    if (m_uiMaxKernelCount != DEFAULT_MAX_KERNELS)
    {
        KernelProfileResultManager::Instance()->AddHeader(StringUtils::FormatString("%s=%d", FILE_HEADER_MAX_NUMBER_OF_KERNELS_TO_PROFILE, m_uiMaxKernelCount));
    }

    KernelProfileResultManager::Instance()->AddProfileResultItem(CSV_COMMON_COLUMN_METHOD);
    KernelProfileResultManager::Instance()->AddProfileResultItem(CSV_COMMON_COLUMN_EXECUTION_ORDER);
    KernelProfileResultManager::Instance()->AddProfileResultItem(CSV_COMMON_COLUMN_THREAD_ID);
    KernelProfileResultManager::Instance()->AddProfileResultItem(CSV_COMMON_COLUMN_CALL_INDEX);
    KernelProfileResultManager::Instance()->AddProfileResultItem(CSV_COMMON_COLUMN_GLOBAL_WORK_SIZE);
    KernelProfileResultManager::Instance()->AddProfileResultItem(CSV_COMMON_COLUMN_WORK_GROUP_SIZE);

    if (m_bCollectGPUTime)
    {
        KernelProfileResultManager::Instance()->AddProfileResultItem(CSV_COMMON_COLUMN_TIME);
    }

    KernelProfileResultManager::Instance()->AddProfileResultItem(CSV_COMMON_COLUMN_LOCAL_MEM_SIZE);
    KernelProfileResultManager::Instance()->AddProfileResultItem(CSV_COMMON_COLUMN_VGPRs);
    KernelProfileResultManager::Instance()->AddProfileResultItem(CSV_COMMON_COLUMN_SGPRs);
    KernelProfileResultManager::Instance()->AddProfileResultItem(CSV_COMMON_COLUMN_SCRATCH_REGS);
}

void CLGPAProfiler::DumpKernelStats(const KernelStats& kernelStats)
{
    ++m_uiOutputLineCount;

    KernelProfileResultManager::Instance()->WriteKernelInfo(CSV_COMMON_COLUMN_METHOD, kernelStats.m_strName);
    KernelProfileResultManager::Instance()->WriteKernelInfo(CSV_COMMON_COLUMN_EXECUTION_ORDER, m_uiOutputLineCount);
    KernelProfileResultManager::Instance()->WriteKernelInfo(CSV_COMMON_COLUMN_THREAD_ID, kernelStats.m_threadId);
    KernelProfileResultManager::Instance()->WriteKernelInfo(CSV_COMMON_COLUMN_CALL_INDEX, kernelStats.m_uSequenceId);

    if (0 == kernelStats.m_globalWorkSize[0] &&
        0 == kernelStats.m_globalWorkSize[1] &&
        0 == kernelStats.m_globalWorkSize[2])
    {
        KernelProfileResultManager::Instance()->WriteKernelInfo(CSV_COMMON_COLUMN_GLOBAL_WORK_SIZE, "NULL");
    }
    else
    {
        KernelProfileResultManager::Instance()->WriteKernelInfo(CSV_COMMON_COLUMN_GLOBAL_WORK_SIZE, StringUtils::FormatString("{%7lu %7lu %7lu}",
                                                                static_cast<unsigned long>(kernelStats.m_globalWorkSize[0]),
                                                                static_cast<unsigned long>(kernelStats.m_globalWorkSize[1]),
                                                                static_cast<unsigned long>(kernelStats.m_globalWorkSize[2])));
    }

    if (0 == kernelStats.m_workGroupSize[0] &&
        0 == kernelStats.m_workGroupSize[1] &&
        0 == kernelStats.m_workGroupSize[2])
    {
        KernelProfileResultManager::Instance()->WriteKernelInfo(CSV_COMMON_COLUMN_WORK_GROUP_SIZE, "NULL");
    }
    else
    {
        KernelProfileResultManager::Instance()->WriteKernelInfo(CSV_COMMON_COLUMN_WORK_GROUP_SIZE, StringUtils::FormatString("{%5lu %5lu %5lu}",
                                                                (unsigned long) kernelStats.m_workGroupSize[0],
                                                                (unsigned long) kernelStats.m_workGroupSize[1],
                                                                (unsigned long) kernelStats.m_workGroupSize[2]));
    }

    if (m_bCollectGPUTime)
    {
        KernelProfileResultManager::Instance()->WriteKernelInfo(CSV_COMMON_COLUMN_TIME, StringUtils::FormatString("%15.5lf", kernelStats.m_dTime));
    }

    KernelProfileResultManager::Instance()->WriteKernelInfo(CSV_COMMON_COLUMN_LOCAL_MEM_SIZE, kernelStats.m_kernelInfo.m_nUsedLDSSize == KERNELINFO_NONE ? "NA" : StringUtils::ToString(kernelStats.m_kernelInfo.m_nUsedLDSSize));
    KernelProfileResultManager::Instance()->WriteKernelInfo(CSV_COMMON_COLUMN_VGPRs, kernelStats.m_kernelInfo.m_nUsedGPRs == KERNELINFO_NONE ? "NA" : StringUtils::ToString(kernelStats.m_kernelInfo.m_nUsedGPRs));
    KernelProfileResultManager::Instance()->WriteKernelInfo(CSV_COMMON_COLUMN_SGPRs, kernelStats.m_kernelInfo.m_nUsedScalarGPRs == KERNELINFO_NONE ? "NA" : StringUtils::ToString(kernelStats.m_kernelInfo.m_nUsedScalarGPRs));
    KernelProfileResultManager::Instance()->WriteKernelInfo(CSV_COMMON_COLUMN_SCRATCH_REGS, kernelStats.m_kernelInfo.m_nScratchReg == KERNELINFO_NONE ? "NA" : StringUtils::ToString(kernelStats.m_kernelInfo.m_nScratchReg));
}

bool CLGPAProfiler::DumpSession(GPA_SessionId sessionId, const KernelStats& kernelStats)
{
    if (!m_gpaUtils.Loaded())
    {
        return false;
    }

    KernelProfileResultManager::Instance()->BeginKernelInfo();

    bool isSessionReady = false;

    if (m_isGPAOpened)
    {
        isSessionReady = GPA_STATUS_OK == m_gpaUtils.StatusCheck(m_gpaUtils.GetGPAFuncTable()->GPA_IsSessionComplete(sessionId));
    }

    // check whether the session is ready and the result has returned from GPU
    if (!isSessionReady)
    {
        // just CPU profiling (time)
        DumpKernelStats(kernelStats);
        KernelProfileResultManager::Instance()->EndKernelInfo();
        return false;
    }

    if (m_isGPAOpened)
    {
        gpa_uint32 sampleCount;
        m_gpaUtils.StatusCheck(m_gpaUtils.GetGPAFuncTable()->GPA_GetSampleCount(sessionId, &sampleCount));

        if (0 == sampleCount)
        {
            SpAssertRet(!"No samples found") false;
        }

        size_t sampleResultSizeInBytes = 0;
        m_gpaUtils.StatusCheck(m_gpaUtils.GetGPAFuncTable()->GPA_GetSampleResultSize(sessionId, 0, &sampleResultSizeInBytes));

        gpa_uint64* pResultsBuffer = reinterpret_cast<gpa_uint64*>(malloc(sampleResultSizeInBytes));
        m_gpaUtils.StatusCheck(m_gpaUtils.GetGPAFuncTable()->GPA_GetSampleResult(sessionId, 0, sampleResultSizeInBytes, pResultsBuffer));

        gpa_uint32 nEnabledCounters = 0;
        m_gpaUtils.GetGPAFuncTable()->GPA_GetNumEnabledCounters(sessionId, &nEnabledCounters);

        for (gpa_uint32 sample = 0; sample < sampleCount; sample++)
        {
            DumpKernelStats(kernelStats);

            for (gpa_uint32 counter = 0; counter < nEnabledCounters; counter++)
            {
                gpa_uint32 enabledCounterIndex;
                m_gpaUtils.StatusCheck(m_gpaUtils.GetGPAFuncTable()->GPA_GetEnabledIndex(sessionId, counter, &enabledCounterIndex));

                GPA_Data_Type dataType;

                if (!m_gpaUtils.GetCounterDataType(enabledCounterIndex, dataType))
                {
                    SpBreak("Failed to retrieve counter data type.");
                    continue;
                }

                string strName;

                if (!m_gpaUtils.GetCounterName(enabledCounterIndex, strName))
                {
                    SpBreak("Failed to retrieve counter name.");
                    continue;
                }

                if (GPA_DATA_TYPE_UINT64 == dataType)
                {
#ifdef _WIN32
                    KernelProfileResultManager::Instance()->WriteKernelInfo(strName, StringUtils::FormatString("%8I64u", pResultsBuffer[counter]));
#else
                    KernelProfileResultManager::Instance()->WriteKernelInfo(strName, StringUtils::FormatString("%lu", pResultsBuffer[counter]));
#endif
                }
                else if (GPA_DATA_TYPE_FLOAT64 == dataType)
                {
                    KernelProfileResultManager::Instance()->WriteKernelInfo(strName, StringUtils::FormatString("%12.2f", reinterpret_cast<gpa_float64*>(pResultsBuffer)[counter]));
                }
                else
                {
                    SpAssertRet(!"Unrecognized data type") false;
                }
            }
        }

        free(pResultsBuffer);
    }

    KernelProfileResultManager::Instance()->EndKernelInfo();
    m_gpaUtils.StatusCheck(m_gpaUtils.GetGPAFuncTable()->GPA_DeleteSession(sessionId));

    return true;
}

void CLGPAProfiler::AddUserEvent(cl_event userEvent)
{
    CLUserEvent* pUserEventWrapper = new(nothrow) CLUserEvent(userEvent);

    if (pUserEventWrapper != NULL)
    {
        m_userEventList.push_back(pUserEventWrapper);
    }
}

bool CLGPAProfiler::HasUserEvent(const cl_event* wait_list, int num, cl_event* event)
{
    for (UserEventList::iterator it = m_userEventList.begin(); it != m_userEventList.end(); it++)
    {
        for (int i = 0; i < num; i++)
        {
            if ((*it)->CheckDependency(wait_list[i]))
            {
                if (event != NULL)
                {
                    (*it)->AddDependentEvent(*event);
                }

                return true;
            }
        }
    }

    return false;
}

CLUserEvent* CLGPAProfiler::HasUserEvent(const cl_event* wait_list, int num)
{
    for (UserEventList::iterator it = m_userEventList.begin(); it != m_userEventList.end(); it++)
    {
        for (int i = 0; i < num; i++)
        {
            if (wait_list[i] == (*it)->m_event)
            {
                return *it;
            }
        }
    }

    return NULL;
}

void CLGPAProfiler::RemoveUserEvent(cl_event event)
{
    UserEventList::iterator found = m_userEventList.end();

    for (UserEventList::iterator it = m_userEventList.begin(); it != m_userEventList.end(); it++)
    {
        if (event == (*it)->m_event)
        {
            found = it;
            break;
        }
    }

    if (found != m_userEventList.end())
    {
        delete(*found);
        m_userEventList.erase(found);
    }
}


bool CLGPAProfiler::IsProfilerDelayEnabled(unsigned long& delayInMilliseconds)
{
    delayInMilliseconds = m_delayInMilliseconds;
    return m_bDelayStartEnabled;
}


bool CLGPAProfiler::IsProfilerDurationEnabled(unsigned long& durationInMilliseconds)
{
    durationInMilliseconds = m_durationInMilliseconds;
    return m_bProfilerDurationEnabled;
}


void CLGPAProfiler::SetTimerFinishHandler(ProfilerTimerType timerType, TimerEndHandler timerEndHandler)
{
    switch (timerType)
    {
        case PROFILEDELAYTIMER:
            if (nullptr != m_pDelayTimer)
            {
                m_pDelayTimer->SetTimerFinishHandler(timerEndHandler);
            }

            break;

        case PROFILEDURATIONTIMER:
            if (nullptr != m_pDurationTimer)
            {
                m_pDurationTimer->SetTimerFinishHandler(timerEndHandler);
            }

            break;

        default:
            break;
    }
}

void CLGPAProfiler::CreateTimer(ProfilerTimerType timerType, unsigned long timeIntervalInMilliseconds)
{
    switch (timerType)
    {
        case PROFILEDELAYTIMER:
            if (m_pDelayTimer == nullptr && timeIntervalInMilliseconds > 0)
            {
                m_pDelayTimer = new(std::nothrow) ProfilerTimer(timeIntervalInMilliseconds);

                if (nullptr == m_pDelayTimer)
                {
                    Log(logERROR, "CreateTimer: unable to allocate memory for delay timer\n");
                }
                else
                {
                    m_pDelayTimer->SetTimerType(PROFILEDELAYTIMER);
                    m_bDelayStartEnabled = true;
                    m_delayInMilliseconds = timeIntervalInMilliseconds;
                }
            }

            break;

        case PROFILEDURATIONTIMER:
            if (m_pDurationTimer == nullptr && timeIntervalInMilliseconds > 0)
            {
                m_pDurationTimer = new(std::nothrow) ProfilerTimer(timeIntervalInMilliseconds);

                if (nullptr == m_pDurationTimer)
                {
                    Log(logERROR, "CreateTimer: unable to allocate memory for duration timer\n");
                }
                else
                {
                    m_pDurationTimer->SetTimerType(PROFILEDURATIONTIMER);
                    m_bProfilerDurationEnabled = true;
                    m_durationInMilliseconds = timeIntervalInMilliseconds;
                }
            }

            break;

        default:
            break;
    }
}


void CLGPAProfiler::StartTimer(ProfilerTimerType timerType)
{
    switch (timerType)
    {
        case PROFILEDELAYTIMER:
            if (nullptr != m_pDelayTimer)
            {
                m_pDelayTimer->startTimer(true);
            }

            break;

        case PROFILEDURATIONTIMER:
            if (nullptr != m_pDurationTimer)
            {
                m_pDurationTimer->startTimer(true);
            }

            break;

        default:
            break;
    }

}

void CLGPAProfiler::SetOutputFile(const std::string& strOutputFile)
{
    if (strOutputFile.empty())
    {
        // If output file is not set, we use exe name as file name
        m_strOutputFile = FileUtils::GetDefaultOutputPath() + FileUtils::GetExeName() + "." + PERF_COUNTER_EXT;
    }
    else
    {
        std::string strExtension("");

        strExtension = FileUtils::GetFileExtension(strOutputFile);

        if (strExtension != PERF_COUNTER_EXT)
        {
            if ((strExtension == TRACE_EXT) || (strExtension == OCCUPANCY_EXT))
            {
                std::string strBaseFileName = FileUtils::GetBaseFileName(strOutputFile);
                m_strOutputFile = strBaseFileName + "." + PERF_COUNTER_EXT;
            }
            else
            {
                m_strOutputFile = strOutputFile + "." + PERF_COUNTER_EXT;
            }
        }
        else
        {
            m_strOutputFile = strOutputFile;
        }
    }

    KernelProfileResultManager::Instance()->SetOutputFile(m_strOutputFile);
}

bool CLGPAProfiler::GetAvailableDeviceIdFromDeviceNameAndAsicInfoList(const char* pszDeviceName, const AsicInfoList asicInfoList, int& deviceId, int& revisionId)
{
    bool retVal = false;
    deviceId = -1;
    revisionId = -1;

    for (AsicInfoList::const_iterator it = asicInfoList.begin(); it != asicInfoList.end(); ++it)
    {
        GDT_GfxCardInfo cardInfo;
        int curDeviceId = it->deviceID;
        int curRevisionId = it->revID;

        if (AMDTDeviceInfoUtils::Instance()->GetDeviceInfo(curDeviceId, curRevisionId, cardInfo))
        {
            if (strcmp(cardInfo.m_szCALName, pszDeviceName) == 0)
            {
                retVal = true;
                deviceId = curDeviceId;
                revisionId = curRevisionId;
                break;
            }
        }
    }

    return retVal;
}

