//==============================================================================
// Copyright (c) 2015-2018 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  This file contains functions called by various intercepted APIs
//==============================================================================

#include "Logger.h"
#include "StringUtils.h"

#include "HSAToolsRTModule.h"
#include "ROCProfilerModule.h"
#include "HSARTModuleLoader.h"
#include "FinalizerInfoManager.h"
#include "HSAGPAProfiler.h"
#include "HSAPMCInterceptionHelpers.h"
#include "AutoGenerated/HSAPMCInterception.h"
#include "GPUPerfAPI-ROCm.h"

typedef decltype(SetROCmSetContextCallback)*       SetROCmSetContextCallbackFunc;       ///< typedef for a function to register a GPA callback
typedef decltype(SetROCmDispatchCompleteCallback)* SetROCmDispatchCompleteCallbackFunc; ///< typedef for a function to register a GPA callback

// Singleton class to manage global state for the interception helpers
class PMCInterceptionHelpersState : public TSingleton<PMCInterceptionHelpersState>
{
    friend class TSingleton<PMCInterceptionHelpersState>;
public:

    rocprofiler_t*                      m_pContext                             = nullptr; ///< ROCprofiler context
    LIB_HANDLE                          m_gpaModule                            = nullptr; ///< GPA moduel handle
    SetROCmSetContextCallbackFunc       m_pSetROCmSetContextCallbackFunc       = nullptr; ///< GPA callback function
    SetROCmDispatchCompleteCallbackFunc m_pSetROCmDispatchCompleteCallbackFunc = nullptr; ///< GPA callback function

    /// Initialize GPA's SetCallback functions
    /// \return true if the GPA SetCallback functions could be initialized
    bool InitGPASetCallbackFunctions();
};

bool PMCInterceptionHelpersState::InitGPASetCallbackFunctions()
{
    if (nullptr == m_gpaModule)
    {
#ifdef UNICODE
        LocaleString gpaPath = GlobalSettings::GetInstance()->m_params.m_strDLLPath.asCharArray();
        LocaleString gpaLib = GPAApiManager::Instance()->GetLibraryFullPath(GPA_API_ROCM, gpaPath);
        std::string utf8GpaLib;
        StringUtils::WideStringToUtf8String(gpaLib, utf8GpaLib);
        m_gpaModule = OSUtils::Instance()->GetLibraryHandle(utf8GpaLib.c_str());
#else
        LocaleString gpaPath(GlobalSettings::GetInstance()->m_params.m_strDLLPath.asASCIICharArray());
        LocaleString gpaLib = GPAApiManager::Instance()->GetLibraryFullPath(GPA_API_ROCM, gpaPath);
        m_gpaModule = OSUtils::Instance()->GetLibraryHandle(gpaLib.c_str());
#endif
        if (nullptr != m_gpaModule)
        {
            m_pSetROCmSetContextCallbackFunc = reinterpret_cast<SetROCmSetContextCallbackFunc>(OSUtils::Instance()->GetSymbolAddr(m_gpaModule, "SetROCmSetContextCallback"));

            if (nullptr == m_pSetROCmSetContextCallbackFunc)
            {
                Log(logERROR, "Unable to get address of SetROCmSetContextCallback from GPUPerfAPI\n");
            }

            m_pSetROCmDispatchCompleteCallbackFunc = reinterpret_cast<SetROCmDispatchCompleteCallbackFunc>(OSUtils::Instance()->GetSymbolAddr(m_gpaModule, "SetROCmDispatchCompleteCallback"));

            if (nullptr == m_pSetROCmDispatchCompleteCallbackFunc)
            {
                Log(logERROR, "Unable to get address of SetROCmDispatchCompleteCallback from GPUPerfAPI\n");
            }
        }
        else
        {
            Log(logERROR, "Unable to get GPA module handle\n");
        }
    }

    return (nullptr != m_gpaModule &&
            nullptr != m_pSetROCmSetContextCallbackFunc &&
            nullptr != m_pSetROCmDispatchCompleteCallbackFunc);
}

hsa_status_t HSA_PMC_hsa_executable_iterate_agent_symbols_Callback(hsa_executable_t exec, hsa_agent_t agent, hsa_executable_symbol_t symbol, void *data)
{
    SP_UNREFERENCED_PARAMETER(agent);
    SP_UNREFERENCED_PARAMETER(data);

    uint64_t kernelObject;
    hsa_status_t ret;

    ret = g_pRealCoreFunctions->hsa_executable_symbol_get_info_fn(symbol, HSA_EXECUTABLE_SYMBOL_INFO_KERNEL_OBJECT, &kernelObject);

    if (HSA_STATUS_SUCCESS == ret)
    {
        Log(logMESSAGE, "HSA_PMC_hsa_executable_load_agent_code_object: Adding kernel code handle/executable handle pair to FinalizerInfoManager\n");
        Log(logMESSAGE, "  CodeHandle: %llu, ExeHandle: %llu\n", kernelObject, exec.handle);
        FinalizerInfoManager::Instance()->m_kernelObjHandleToExeHandleMap[kernelObject] = exec.handle;
    }

    return ret;
}

void HSA_PMC_ROCP_DispatchCompleteCallback(void)
{
    Log(logMESSAGE, "HSA_PMC_ROCP_DispatchCompleteCallback called\n");

    // TODO: potential race condition where profiling enabled state changes in between dispatch and dispatch-complete callbacks
    if (HSAGPAProfiler::Instance()->IsProfilingEnabled())
    {
        // TODO: assert that pRTParam is not null and pRTParam->pre_dispatch is false
        bool ret = HSAGPAProfiler::Instance()->End();

        if (!ret)
        {
            return;
        }
    }
}

void HSA_PMC_ROCP_SetContextCallback(rocprofiler_t* pContext)
{
    Log(logMESSAGE, "HSA_PMC_ROCP_SetContextCallback called\n");
    SpAssert(nullptr == PMCInterceptionHelpersState::Instance()->m_pContext);

    PMCInterceptionHelpersState::Instance()->m_pContext = pContext;
}

// This is here to work around SWDEV-170322 -- queue destruction fails if a queue destroy callback is not registered
hsa_status_t HSA_PMC_ROCP_QueueDestroyCallback(hsa_queue_t*, void*)
{
    // no implementation needed
    return HSA_STATUS_SUCCESS;
}

hsa_status_t HSA_PMC_ROCP_DispatchCallback(const rocprofiler_callback_data_t* pCallbackData, void* pUserData, rocprofiler_group_t* pGroup)
{
    SP_UNREFERENCED_PARAMETER(pUserData);

    Log(logMESSAGE, "HSA_PMC_ROCP_DispatchCallback called\n");

    hsa_status_t retVal = HSA_STATUS_ERROR;

    ROCProfilerModule* pROCProfilerModule = HSARTModuleLoader<ROCProfilerModule>::Instance()->GetHSARTModule();

    if (nullptr == pROCProfilerModule || !pROCProfilerModule->IsModuleLoaded())
    {
        Log(logERROR, "ROC Profiler module is not loaded\n");
    }
    else
    {
        if (HSAGPAProfiler::Instance()->HasKernelMaxBeenReached())
        {
            retVal = pROCProfilerModule->rocprofiler_remove_queue_callbacks();
        }
        else if (HSAGPAProfiler::Instance()->IsProfilingEnabled())
        {
            if (PMCInterceptionHelpersState::Instance()->InitGPASetCallbackFunctions())
            {
                PMCInterceptionHelpersState::Instance()->m_pSetROCmSetContextCallbackFunc(HSA_PMC_ROCP_SetContextCallback);

                bool ret = HSAGPAProfiler::Instance()->Begin(pCallbackData);

                if (ret)
                {
                    PMCInterceptionHelpersState::Instance()->m_pSetROCmDispatchCompleteCallbackFunc(HSA_PMC_ROCP_DispatchCompleteCallback);
                }

                rocprofiler_t*& pContext = PMCInterceptionHelpersState::Instance()->m_pContext;

                if (nullptr != pContext)
                {
                    uint32_t groupCount = 0;
                    retVal = pROCProfilerModule->rocprofiler_group_count(pContext, &groupCount);
                    assert(groupCount == 1);

                    if (HSA_STATUS_SUCCESS == retVal)
                    {
                        const uint32_t groupIndex = 0;
                        retVal = pROCProfilerModule->rocprofiler_get_group(pContext, groupIndex, pGroup);

                        if (HSA_STATUS_SUCCESS != retVal)
                        {
                            Log(logERROR, "Unable to retrieve the rocprofiler group\n");;
                        }
                    }
                    else
                    {
                        Log(logERROR, "Unable to query rocprofiler group count\n");
                    }

                    pContext = nullptr;
                }
            }
        }
    }

    return retVal;
}

void HSA_PMC_hsa_queue_create_PostCallHelper(hsa_status_t retVal, hsa_agent_t agent, uint32_t size, hsa_queue_type32_t type, void(*callback)(hsa_status_t status, hsa_queue_t* source,
                                             void* data), void* data, uint32_t private_segment_size, uint32_t group_segment_size, hsa_queue_t** queue)
{
    SP_UNREFERENCED_PARAMETER(agent);
    SP_UNREFERENCED_PARAMETER(size);
    SP_UNREFERENCED_PARAMETER(type);
    SP_UNREFERENCED_PARAMETER(callback);
    SP_UNREFERENCED_PARAMETER(data);
    SP_UNREFERENCED_PARAMETER(private_segment_size);
    SP_UNREFERENCED_PARAMETER(group_segment_size);

    if (HSA_STATUS_SUCCESS == retVal && NULL != queue)
    {
        ROCProfilerModule* pROCProfilerModule = HSARTModuleLoader<ROCProfilerModule>::Instance()->GetHSARTModule();

        if (nullptr == pROCProfilerModule || !pROCProfilerModule->IsModuleLoaded())
        {
            Log(logERROR, "ROC Profiler module is not loaded\n");
        }
        else
        {
            // TODO: since queue callbacks are global (not per-queue), we only really need to set these once, not every time a queue is created
            rocprofiler_queue_callbacks_t queueCallbacks { nullptr, nullptr, nullptr };
            queueCallbacks.dispatch = HSA_PMC_ROCP_DispatchCallback;
            queueCallbacks.destroy = HSA_PMC_ROCP_QueueDestroyCallback;

            hsa_status_t status = pROCProfilerModule->rocprofiler_set_queue_callbacks(queueCallbacks, nullptr);

            if (HSA_STATUS_SUCCESS != status)
            {
                Log(logERROR, "Unable to set queue callbacks\n");
            }
        }
    }
}

void HSA_PMC_hsa_queue_destroy_PreCallHelper(hsa_queue_t* queue)
{
    if (NULL != queue)
    {
        HSAGPAProfiler::Instance()->WaitForCompletedSession(queue->id);
    }
}

void HSA_PMC_hsa_executable_get_symbol_PostCallHelper(hsa_status_t retVal, hsa_executable_t executable, const char* module_name, const char* symbol_name, hsa_agent_t agent, int32_t call_convention, hsa_executable_symbol_t* symbol)
{
    SP_UNREFERENCED_PARAMETER(module_name);
    SP_UNREFERENCED_PARAMETER(agent);
    SP_UNREFERENCED_PARAMETER(call_convention);

    if (HSA_STATUS_SUCCESS == retVal && NULL != symbol)
    {
        if (NULL != symbol_name)
        {
            Log(logMESSAGE, "HSA_PMC_hsa_executable_get_symbol: Adding symbol handle/symbol name pair to FinalizerInfoManager\n");
            Log(logMESSAGE, "  SymHandle: %llu, SymName: %s \n", symbol->handle, symbol_name);
            FinalizerInfoManager::Instance()->m_symbolHandleToNameMap[symbol->handle] = std::string(symbol_name);

            uint64_t kernelObject;

            if (g_pRealCoreFunctions->hsa_executable_symbol_get_info_fn(*symbol, HSA_EXECUTABLE_SYMBOL_INFO_KERNEL_OBJECT, &kernelObject) == HSA_STATUS_SUCCESS)
            {
                Log(logMESSAGE, "HSA_PMC_hsa_executable_get_symbol: Adding kernel code handle/symbol handle pair to FinalizerInfoManager\n");
                Log(logMESSAGE, "  CodeHandle: %llu, SymHandle: %llu\n", kernelObject, symbol->handle);
                FinalizerInfoManager::Instance()->m_codeHandleToSymbolHandleMap[kernelObject] = symbol->handle;
                FinalizerInfoManager::Instance()->m_kernelObjHandleToExeHandleMap[kernelObject] = executable.handle;
            }
        }
    }
}

void HSA_PMC_hsa_executable_get_symbol_by_name_PostCallHelper(hsa_status_t retVal, hsa_executable_t executable, const char* symbol_name, const hsa_agent_t* agent, hsa_executable_symbol_t* symbol)
{
    SP_UNREFERENCED_PARAMETER(agent);

    if (HSA_STATUS_SUCCESS == retVal && NULL != symbol)
    {
        if (NULL != symbol_name)
        {
            Log(logMESSAGE, "HSA_PMC_hsa_executable_get_symbol_by_name: Adding symbol handle/symbol name pair to FinalizerInfoManager\n");
            Log(logMESSAGE, "  SymHandle: %llu, SymName: %s \n", symbol->handle, symbol_name);
            FinalizerInfoManager::Instance()->m_symbolHandleToNameMap[symbol->handle] = std::string(symbol_name);

            uint64_t kernelObject;

            if (g_pRealCoreFunctions->hsa_executable_symbol_get_info_fn(*symbol, HSA_EXECUTABLE_SYMBOL_INFO_KERNEL_OBJECT, &kernelObject) == HSA_STATUS_SUCCESS)
            {
                Log(logMESSAGE, "HSA_PMC_hsa_executable_get_symbol_by_name: Adding kernel code handle/symbol handle pair to FinalizerInfoManager\n");
                Log(logMESSAGE, "  CodeHandle: %llu, SymHandle: %llu\n", kernelObject, symbol->handle);
                Log(logMESSAGE, "HSA_PMC_hsa_executable_get_symbol_by_name: Adding kernel code handle/executable handle pair to FinalizerInfoManager\n");
                Log(logMESSAGE, "  CodeHandle: %llu, ExeHandle: %llu\n", kernelObject, executable.handle);
                FinalizerInfoManager::Instance()->m_codeHandleToSymbolHandleMap[kernelObject] = symbol->handle;
                FinalizerInfoManager::Instance()->m_kernelObjHandleToExeHandleMap[kernelObject] = executable.handle;
            }
        }
    }
}

void HSA_PMC_hsa_executable_symbol_get_info_PostCallHelper(hsa_status_t retVal, hsa_executable_symbol_t executable_symbol, hsa_executable_symbol_info_t attribute, void* value)
{
    SP_UNREFERENCED_PARAMETER(attribute);

    if (HSA_STATUS_SUCCESS == retVal && nullptr != value)
    {
        uint32_t symbolNameLength = 0;

        if (g_pRealCoreFunctions->hsa_executable_symbol_get_info_fn(executable_symbol, HSA_EXECUTABLE_SYMBOL_INFO_NAME_LENGTH, &symbolNameLength) == HSA_STATUS_SUCCESS)
        {
            if (symbolNameLength > 0)
             {
                char* pTempSymbolName = new(std::nothrow) char[symbolNameLength + 1];

                if (nullptr != pTempSymbolName)
                {
                    memcpy(pTempSymbolName, value, symbolNameLength);
                    pTempSymbolName[symbolNameLength] = '\0';
                    Log(logMESSAGE, "HSA_PMC_hsa_executable_symbol_get_info: Adding symbol handle/symbol name pair to FinalizerInfoManager\n");
                    Log(logMESSAGE, "  SymHandle: %llu, SymName: %s \n", executable_symbol.handle, pTempSymbolName);
                    FinalizerInfoManager::Instance()->m_symbolHandleToNameMap[executable_symbol.handle] = std::string(pTempSymbolName);
                    delete[] pTempSymbolName;

                    uint64_t kernelObject;

                    if (g_pRealCoreFunctions->hsa_executable_symbol_get_info_fn(executable_symbol, HSA_EXECUTABLE_SYMBOL_INFO_KERNEL_OBJECT, &kernelObject) == HSA_STATUS_SUCCESS)
                    {
                        Log(logMESSAGE, "HSA_PMC_hsa_executable_symbol_get_info: Adding kernel code handle/symbol handle pair to FinalizerInfoManager\n");
                        Log(logMESSAGE, "  CodeHandle: %llu, ExecutableHandle: %llu\n", kernelObject, executable_symbol.handle);
                        FinalizerInfoManager::Instance()->m_codeHandleToSymbolHandleMap[kernelObject] = executable_symbol.handle;
                    }
                }
            }
        }
    }
}

void HSA_PMC_hsa_executable_load_code_object_PostCallHelper(hsa_status_t retVal, hsa_executable_t executable, hsa_agent_t agent, hsa_code_object_t code_object, const char* options)
{
    SP_UNREFERENCED_PARAMETER(options);

    if (HSA_STATUS_SUCCESS == retVal)
    {
        FinalizerInfoManager::Instance()->m_exeAndAgentHandleToCodeObjHandleMap[std::make_pair(executable.handle, agent.handle)] = code_object.handle;
    }
}

void HSA_PMC_hsa_executable_load_agent_code_object_PostCallHelper(hsa_status_t retVal, hsa_executable_t executable, hsa_agent_t agent, hsa_code_object_reader_t code_object_reader, const char* options, hsa_loaded_code_object_t* loaded_code_object)
{
    SP_UNREFERENCED_PARAMETER(code_object_reader);
    SP_UNREFERENCED_PARAMETER(options);

    if (HSA_STATUS_SUCCESS == retVal && nullptr != loaded_code_object)
    {
        FinalizerInfoManager::Instance()->m_exeAndAgentHandleToLoadedCodeObjHandleMap[std::make_pair(executable.handle, agent.handle)] = loaded_code_object->handle;

        if (HSA_STATUS_SUCCESS != g_pRealCoreFunctions->hsa_executable_iterate_agent_symbols_fn(executable, agent, HSA_PMC_hsa_executable_iterate_agent_symbols_Callback, nullptr))
        {
            Log(logERROR, "Failed to iterate symbols using hsa_executable_iterate_agent_symbols\n");
        }
    }
}
