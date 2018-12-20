//==============================================================================
// Copyright (c) 2015 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Intercepted OpenCL APIs
//==============================================================================

#include <AMDTOSWrappers/Include/osThread.h>

#include "Logger.h"
#include "CLIntercept.h"
#include "CLOccupancyInfoManager.h"
#include "../CLCommon/CLFunctionDefs.h"
#include "../Common/OSUtils.h"
#include "../CLCommon/CLUtils.h"
#include "DeviceInfoUtils.h"
#include "CLDeviceReplacer.h"

using namespace GPULogger;


const unsigned int KERNEL_MAX_DIM = 3;

cl_int CL_API_CALL
CL_OCCUPANCY_API_ENTRY_EnqueueNDRangeKernel(
    cl_command_queue cq,
    cl_kernel        kernel,
    cl_uint          wgDim,
    const size_t*    globalOffset,
    const size_t*    globalWS,
    const size_t*    localWS,
    cl_uint          nEventsInWaitList,
    const cl_event*  pEventList,
    cl_event*        pEvent)
{
    // call original API
    cl_int status = g_nextDispatchTable.EnqueueNDRangeKernel(cq,
                                                             kernel,
                                                             wgDim,
                                                             globalOffset,
                                                             globalWS,
                                                             localWS,
                                                             nEventsInWaitList,
                                                             pEventList,
                                                             pEvent);

    if (!OccupancyInfoManager::Instance()->IsProfilingEnabled())
    {
        // profiling is disabled by AMDTActivityLogger
        return status;
    }

    if (CL_SUCCESS != status)
    {
        return status;
    }

    cl_device_id device = NULL;
    cl_int occupancy_status = g_realDispatchTable.GetCommandQueueInfo(cq, CL_QUEUE_DEVICE, sizeof(cl_device_id), &device, NULL);

    if (CL_SUCCESS != occupancy_status)
    {
        Log(logERROR, "Unable to query the command queue device\n");
        return status;
    }

    cl_device_type deviceType;
    occupancy_status = g_realDispatchTable.GetDeviceInfo(device, CL_DEVICE_TYPE, sizeof(cl_device_type), &deviceType, NULL);

    if (CL_SUCCESS != occupancy_status)
    {
        Log(logERROR, "Unable to query the device type\n");
        return status;
    }

    if (CL_DEVICE_TYPE_GPU != deviceType)
    {
        Log(logMESSAGE, "Device is CPU.  Occupancy not supported\n");
        // Ignore CPU device
        return status;
    }

    // Query the kernel name
    const size_t KERNEL_NAME_BUFFER_SIZE = 256;
    char szKernelNameBuffer[KERNEL_NAME_BUFFER_SIZE];
    occupancy_status = g_realDispatchTable.GetKernelInfo(kernel, CL_KERNEL_FUNCTION_NAME, KERNEL_NAME_BUFFER_SIZE, szKernelNameBuffer, NULL);

    std::string strKernelName;

    if (CL_SUCCESS != occupancy_status)
    {
        Log(logERROR, "Unable to query the kernel name\n");
        strKernelName = "UNKNOWN_KERNEL";
    }
    else
    {
        strKernelName = szKernelNameBuffer;
    }

    KernelFilterList enabledKernels = GlobalSettings::GetInstance()->m_params.m_kernelFilterList;

    bool skipProfiling = (!enabledKernels.empty()) && (enabledKernels.find(strKernelName) == enabledKernels.end());

    if (skipProfiling)
    {
        return status;
    }

    //get the thread ID
    osThreadId tid = osGetUniqueCurrentThreadId();

    OccupancyInfoEntry* pEntry = new(std::nothrow) OccupancyInfoEntry();
    SpAssertRet(pEntry != NULL) status;

    pEntry->m_tid = tid;
    pEntry->m_strKernelName = strKernelName;

    // Get device name
    if (CLUtils::GetDeviceName(device, pEntry->m_strDeviceName) != CL_SUCCESS)
    {
        Log(logERROR, "Unable to get the device name\n");
        SAFE_DELETE(pEntry);
        return status;
    }

    cl_uint gfxIpMajor = 0;
    occupancy_status = g_realDispatchTable.GetDeviceInfo(device, CL_DEVICE_GFXIP_MAJOR_AMD, sizeof(cl_uint), &gfxIpMajor, NULL);

    if (CL_SUCCESS != occupancy_status)
    {
        Log(logERROR, "Unable to query the gfxip major version\n");
        return occupancy_status;
    }

    if (0 == gfxIpMajor)
    {
        // workaround failure of CL_DEVICE_GFXIP_MAJOR_AMD on ROCm by extracting the major gfx ip ver from the device name
        char szDeviceName[SP_MAX_PATH];
        occupancy_status = g_realDispatchTable.GetDeviceInfo(device, CL_DEVICE_NAME, SP_MAX_PATH, szDeviceName, NULL);

        if (occupancy_status != CL_SUCCESS && (szDeviceName[0] == 'g' && szDeviceName[1] == 'f' && szDeviceName[2] == 'x'))
        {
            gfxIpMajor = szDeviceName[3] - '0';

            if (8 > gfxIpMajor)
            {
                gfxIpMajor = (10 * gfxIpMajor) + szDeviceName[4] - '0';
            }

            if (0 == gfxIpMajor)
            {
                Log(logERROR, "gfxip major version is zero\n");
                return CL_INVALID_VALUE;
            }
        }
        else
        {
            Log(logERROR, "Unable to query the gfxip major version\n");
            return CL_INVALID_VALUE;
        }
    }

    pEntry->m_nDeviceGfxIpVer = gfxIpMajor;

    GDT_HW_GENERATION gen = GDT_HW_GENERATION_NONE;
    cl_uint pcieID;
    bool isHwGenSet = false;
    bool isPCIEDeviceIDSet = false;

    if (CL_SUCCESS == g_realDispatchTable.GetDeviceInfo(device, CL_DEVICE_PCIE_ID_AMD, sizeof(cl_uint), &pcieID, nullptr) && 0 != pcieID)
    {
        isHwGenSet = AMDTDeviceInfoUtils::Instance()->GetHardwareGeneration(pcieID, gen);
        isPCIEDeviceIDSet = true;
    }

    if (!isHwGenSet)
    {
        if (!AMDTDeviceInfoUtils::Instance()->GetHardwareGeneration(pEntry->m_strDeviceName.c_str(), gen))
        {
            Log(logERROR, "Unable to query the hw generation\n");
            SAFE_DELETE(pEntry);
            return status;
        }
    }

    if (gen >= GDT_HW_GENERATION_VOLCANICISLAND && gen < GDT_HW_GENERATION_LAST)
    {
        pEntry->m_pCLCUInfo = new(std::nothrow) CLCUInfoVI();
    }
    else if (gen == GDT_HW_GENERATION_SOUTHERNISLAND || gen == GDT_HW_GENERATION_SEAISLAND)
    {
        pEntry->m_pCLCUInfo = new(std::nothrow) CLCUInfoSI();
    }
    else
    {
        Log(logERROR, "Unsupported hw generation\n");
        SAFE_DELETE(pEntry);
        return status;
    }

    SpAssertRet(pEntry->m_pCLCUInfo != NULL) status;

    //compute work-group size
    pEntry->m_nWorkGroupItemCount = 1;

    if (localWS == NULL)
    {
        occupancy_status = g_realDispatchTable.GetKernelWorkGroupInfo(kernel, device, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &pEntry->m_nWorkGroupItemCount, NULL);

        if (CL_SUCCESS != occupancy_status)
        {
            Log(logERROR, "Unable to query the kernel workgroup info\n");
            SAFE_DELETE(pEntry);
            return status;
        }
    }
    else
    {
        for (unsigned int i = 0; i < wgDim; ++i)
        {
            pEntry->m_nWorkGroupItemCount *= localWS[i];
        }
    }

    // max work group size
    occupancy_status = g_realDispatchTable.GetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &pEntry->m_nWorkGroupItemCountMax, NULL);

    if (CL_SUCCESS != occupancy_status)
    {
        Log(logERROR, "Unable to query the device max workgroup size\n");
        SAFE_DELETE(pEntry);
        return status;
    }

    // global work-size
    pEntry->m_nGlobalItemCount = 1;

    if (globalWS == NULL)
    {
        pEntry->m_nGlobalItemCount = 0;
    }
    else
    {
        for (unsigned int i = 0; i < wgDim; i++)
        {
            pEntry->m_nGlobalItemCount *= globalWS[i];
        }
    }

    // max global work size
    size_t nDims;
    occupancy_status = g_realDispatchTable.GetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(size_t), &nDims, NULL);

    if (CL_SUCCESS != occupancy_status)
    {
        Log(logWARNING, "Unable to query the device max work item dimensions\n");
        pEntry->m_nGlobalItemCountMax = 0;
    }
    else
    {
        size_t nMaxSize[ KERNEL_MAX_DIM ];
        occupancy_status = g_realDispatchTable.GetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_SIZES, nDims * sizeof(size_t), nMaxSize, NULL);

        if (CL_SUCCESS != occupancy_status)
        {
            Log(logWARNING, "Unable to query the device max work item sizes\n");
            pEntry->m_nGlobalItemCountMax = 0;
        }
        else
        {
            pEntry->m_nGlobalItemCountMax = 1;

            for (unsigned int j = 0; j < nDims; j++)
            {
                pEntry->m_nGlobalItemCountMax *= nMaxSize[j];
            }
        }
    }

    // max number of compute units
    occupancy_status = g_realDispatchTable.GetDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(size_t), &pEntry->m_nNumberOfComputeUnits, NULL);

    if (CL_SUCCESS != occupancy_status)
    {
        Log(logERROR, "Unable to query the device max compute units\n");
        SAFE_DELETE(pEntry);
        return status;
    }

    //compute occupancy

    KernelInfo kinfo;

    if (!CLUtils::QueryKernelInfo(kernel, pEntry->m_strDeviceName, device, kinfo))
    {
        SAFE_DELETE(pEntry);
        Log(logERROR, "Unable to query the kernel info\n");
        return status;
    }

    pEntry->m_pCLCUInfo->SetCUParam(CU_PARAMS_VECTOR_GPRS_MAX, kinfo.m_nAvailableGPRs);
    pEntry->m_pCLCUInfo->SetCUParam(CU_PARAMS_SCALAR_GPRS_MAX, kinfo.m_nAvailableScalarGPRs);
    pEntry->m_pCLCUInfo->SetCUParam(CU_PARAMS_LDS_MAX, kinfo.m_nAvailableLDSSize);

    pEntry->m_pCLCUInfo->SetCUParam(CU_PARAMS_VECTOR_GPRS_USED, kinfo.m_nUsedGPRs);
    pEntry->m_pCLCUInfo->SetCUParam(CU_PARAMS_SCALAR_GPRS_USED, kinfo.m_nUsedScalarGPRs);
    pEntry->m_pCLCUInfo->SetCUParam(CU_PARAMS_LDS_USED, kinfo.m_nUsedLDSSize);

    pEntry->m_pCLCUInfo->SetCUParam(CU_PARAMS_KERNEL_WG_SIZE, pEntry->m_nWorkGroupItemCount);
    pEntry->m_pCLCUInfo->SetCUParam(CU_PARAMS_WG_SIZE_MAX, pEntry->m_nWorkGroupItemCountMax);
    pEntry->m_pCLCUInfo->SetCUParam(CU_PARAMS_KERNEL_GLOBAL_SIZE, pEntry->m_nGlobalItemCount);
    pEntry->m_pCLCUInfo->SetCUParam(CU_PARAMS_GLOBAL_SIZE_MAX, pEntry->m_nGlobalItemCountMax);

    pEntry->m_pCLCUInfo->SetCUParam(CU_PARAMS_NBR_COMPUTE_UNITS, pEntry->m_nNumberOfComputeUnits);

    pEntry->m_pCLCUInfo->SetCUParam(CU_PARAMS_DEVICE_NAME, pEntry->m_strDeviceName);
    pEntry->m_pCLCUInfo->SetCUParam(CU_PARAMS_DEVICE_GFXIP_VER, pEntry->m_nDeviceGfxIpVer);

    GDT_DeviceInfo deviceInfo = {};
    bool isDeviceInfoSet = false;

    if (isPCIEDeviceIDSet)
    {
        isDeviceInfoSet = AMDTDeviceInfoUtils::Instance()->GetDeviceInfo(pcieID, REVISION_ID_ANY, deviceInfo);
    }

    if (!isDeviceInfoSet)
    {
        isDeviceInfoSet = AMDTDeviceInfoUtils::Instance()->GetDeviceInfo(pEntry->m_strDeviceName.c_str(), deviceInfo);
    }

    if (isDeviceInfoSet)
    {
        pEntry->m_nSimdsPerCU = deviceInfo.m_nNumSIMDPerCU;
        pEntry->m_nMaxWavefrontsPerCU = deviceInfo.m_nMaxWavePerSIMD * deviceInfo.m_nNumSIMDPerCU;
        pEntry->m_nWavefrontSize = deviceInfo.m_nWaveSize;
    }
    else
    {
        Log(logWARNING, "Unrecognized device, some device parameters may be incorrect\n");
        pEntry->m_nSimdsPerCU = 4; // for now all ASICs have 4 SIMDS per CU
        pEntry->m_nMaxWavefrontsPerCU = 40; // this will be wrong for some ASICs (some ASICs have 32 wavesfronts per CU, not 40)
        pEntry->m_nWavefrontSize = 64; // for now all ASICs have a wavefront size of 64
    }

    pEntry->m_pCLCUInfo->SetCUParam(CU_PARAMS_SIMDS_PER_CU, pEntry->m_nSimdsPerCU);
    pEntry->m_pCLCUInfo->SetCUParam(CU_PARAMS_WAVEFRONT_PER_COMPUTE_UNIT, pEntry->m_nMaxWavefrontsPerCU);
    pEntry->m_pCLCUInfo->SetCUParam(CU_PARAMS_WAVEFRONT_SIZE, pEntry->m_nWavefrontSize);

    int computeOccupancyStatus = pEntry->m_pCLCUInfo->ComputeCUOccupancy((unsigned int)pEntry->m_nWorkGroupItemCount);

    if (AMD_CUPARAMS_LOADED != computeOccupancyStatus)
    {
        Log(logERROR, "Unable to compute occupancy\n");
        SAFE_DELETE(pEntry);
        return status;
    }

    OccupancyInfoManager::Instance()->AddTraceInfoEntry(pEntry);
    return status;
}

cl_int CL_API_CALL
CL_OCCUPANCY_API_ENTRY_ReleaseContext(cl_context  context)
{
    // call entry in next dispatch table
    cl_int ret = g_nextDispatchTable.ReleaseContext(context);

    // In timeout mode, flush from clReleaseContext (just like in trace agent)
    if (OccupancyInfoManager::Instance()->IsTimeOutMode())
    {
        OccupancyInfoManager::Instance()->StopTimer();
        OccupancyInfoManager::Instance()->TrySwapBuffer();
        OccupancyInfoManager::Instance()->FlushTraceData(true);
        OccupancyInfoManager::Instance()->TrySwapBuffer();
        OccupancyInfoManager::Instance()->FlushTraceData(true);
        OccupancyInfoManager::Instance()->ResumeTimer();
    }

    return ret;
}

cl_int CL_API_CALL
CL_OCCUPANCY_API_ENTRY_GetPlatformInfo(
    cl_platform_id    platform,
    cl_platform_info  param_name,
    size_t            param_value_size,
    void*             param_value,
    size_t*           param_value_size_ret)
{
    cl_int ret = g_nextDispatchTable.GetPlatformInfo(
        platform,
        param_name,
        param_value_size,
        param_value,
        param_value_size_ret);

    if (CL_SUCCESS == ret)
    {
        CLUtils::AddPlatform(platform);
    }

    return ret;
}

cl_int CL_API_CALL
CL_OCCUPANCY_API_ENTRY_GetDeviceIDs(
    cl_platform_id    platform,
    cl_device_type    device_type,
    cl_uint           num_entries,
    cl_device_id*     device_list,
    cl_uint*          num_devices)
{

    cl_int ret = CL_SUCCESS;

    if (((device_type & CL_DEVICE_TYPE_GPU) == CL_DEVICE_TYPE_GPU) &&
        GlobalSettings::GetInstance()->m_params.m_bForceSingleGPU &&
        0u <= GlobalSettings::GetInstance()->m_params.m_uiForcedGpuIndex)
    {
        ret = CLDeviceReplacer::ReplaceDeviceIdsInclGetDeviceIds(
                  platform,
                  device_type,
                  num_entries,
                  device_list,
                  num_devices,
                  GlobalSettings::GetInstance()->m_params.m_uiForcedGpuIndex);
    }
    else
    {
        ret = g_nextDispatchTable.GetDeviceIDs(
                  platform,
                  device_type,
                  num_entries,
                  device_list,
                  num_devices);
    }

    if (CL_SUCCESS == ret)
    {
        CLUtils::AddPlatform(platform);
    }

    return ret;
}


cl_int CL_API_CALL
CL_OCCUPANCY_API_ENTRY_GetContextInfo(
    cl_context          context,
    cl_context_info     param_name,
    size_t              param_value_size,
    void*               param_value,
    size_t*             param_value_size_ret)
{
    cl_int ret = CL_SUCCESS;

    if ((CL_CONTEXT_DEVICES == param_name || CL_CONTEXT_NUM_DEVICES == param_name) &&
        GlobalSettings::GetInstance()->m_params.m_bForceSingleGPU &&
        0u <= GlobalSettings::GetInstance()->m_params.m_uiForcedGpuIndex)
    {
        ret = CLDeviceReplacer::ReplaceDeviceIdsInclGetContextInfo(
                  context,
                  param_name,
                  param_value_size,
                  param_value,
                  param_value_size_ret,
                  GlobalSettings::GetInstance()->m_params.m_uiForcedGpuIndex);
    }
    else
    {
        ret = g_nextDispatchTable.GetContextInfo(
                  context,
                  param_name,
                  param_value_size,
                  param_value,
                  param_value_size_ret);
    }

    return ret;
}


