//==============================================================================
// Copyright (c) 2015 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief This file contains utility functions for OpenCL API.
//==============================================================================

#include <CL/opencl.h>

#include <cstdio>
#include <cstring>
#include <mutex>

#include "CLUtils.h"
#include "CLFunctionDefs.h"
#include "CLInternalFunctionDefs.h"
#include "DeviceInfoUtils.h"
#include "Defs.h"
#include "Logger.h"
#include "CLPlatformInfo.h"

#define INVALID_CL_PLATFORM reinterpret_cast<cl_platform_id>(-1)

static cl_platform_id gs_defaultPlatform = INVALID_CL_PLATFORM;
static std::mutex gs_platformMutex;

cl_int CLUtils::GetDeviceName(cl_device_id device, std::string& strDeviceNameOut)
{
    strDeviceNameOut = "";

    char pszDeviceName[256];
    cl_int result = g_realDispatchTable.GetDeviceInfo(device, CL_DEVICE_NAME, sizeof(pszDeviceName), pszDeviceName, nullptr);

    if (CL_SUCCESS == result)
    {
        strDeviceNameOut = std::string(pszDeviceName);
    }

    return result;
}

bool CLUtils::IsDeviceType(cl_device_id device, cl_device_type deviceType)
{
    bool bRetVal = false;

    cl_device_type tempDeviceType;

    if (CL_SUCCESS == g_realDispatchTable.GetDeviceInfo(device,
                                                        CL_DEVICE_TYPE,
                                                        sizeof(cl_device_type),
                                                        &tempDeviceType,
                                                        nullptr))
    {
        if (deviceType == tempDeviceType)
        {
            bRetVal = true;
        }
    }

    return bRetVal;
}

bool CLUtils::HasDeviceType(const cl_context& context, cl_device_type deviceType)
{
    bool bRetVal = false;

    // get the size of the device list
    size_t deviceListSize;
    cl_int status = g_realDispatchTable.GetContextInfo(context,
                                                       CL_CONTEXT_DEVICES,
                                                       0,
                                                       nullptr,
                                                       &deviceListSize);

    cl_device_id* pDevices = new(std::nothrow) cl_device_id[deviceListSize];
    SpAssert(nullptr != pDevices);

    if (nullptr != pDevices)
    {
        // get the device list
        status |= g_realDispatchTable.GetContextInfo(context,
                                                     CL_CONTEXT_DEVICES,
                                                     deviceListSize,
                                                     pDevices,
                                                     nullptr);

        if (CL_SUCCESS == status)
        {
            bRetVal = HasDeviceType(static_cast<cl_uint>(deviceListSize),
                                    pDevices,
                                    deviceType);
        }
    }

    delete[] pDevices;
    return bRetVal;
}

bool CLUtils::HasDeviceType(cl_uint             nDevices,
                            const cl_device_id* pDevices,
                            cl_device_type      deviceType)
{
    bool bRetVal = false;

    if (nullptr != pDevices)
    {
        for (cl_uint i = 0; i < nDevices; ++i)
        {
            if (IsDeviceType(pDevices[i], deviceType))
            {
                bRetVal = true;
                break;
            }
        }
    }

    return bRetVal;
}

bool CLUtils::GetElapsedTimeFromEvent(const cl_event* pEvent, double& dTimeOut)
{
    bool bRetVal = false;

    if (nullptr != pEvent)
    {
        // wait for the memory call to finish execution
        cl_int status = g_realDispatchTable.WaitForEvents(1, pEvent);

        cl_ulong llStartTime;
        cl_ulong llEndTime;
        status |= g_realDispatchTable.GetEventProfilingInfo(*pEvent,
                                                            CL_PROFILING_COMMAND_START,
                                                            sizeof(cl_ulong),
                                                            &llStartTime,
                                                            nullptr);

        status |= g_realDispatchTable.GetEventProfilingInfo(*pEvent,
                                                            CL_PROFILING_COMMAND_END,
                                                            sizeof(cl_ulong),
                                                            &llEndTime,
                                                            nullptr);

        /* Compute total time (also convert from nanoseconds to milliseconds) */
        dTimeOut = (double)(llEndTime - llStartTime) / 1e6;

        bRetVal = CL_SUCCESS == status;
    }

    return bRetVal;
}

bool CLUtils::GetPlatformInfo(CLPlatformSet& platformList)
{
    bool bRetVal = false;
    cl_int status = CL_SUCCESS;

    CLPlatformInfo::PlatformInfo platformInfo;

    const unsigned int MAX_INFO_BUFFER_SIZE = 512;

    //Query the number of platforms/devices
    cl_uint nDevicesDetected = 0;
    cl_uint nDeviceAddressBits = 0;

    char szInfoBuffer[ MAX_INFO_BUFFER_SIZE ];
    size_t nBytesRead = 0;
    cl_platform_id platform = nullptr;

    if (nullptr != g_realDispatchTable.GetDeviceIDs)
    {
        status = g_realDispatchTable.GetDeviceIDs(nullptr, CL_DEVICE_TYPE_ALL, 0, nullptr, &nDevicesDetected);

        if (CL_SUCCESS != status)
        {
            // Note: the call to clGetDeviceIDs may fail if clGetPlatformIDs is not called first. Adding this call to make sure we can query the devices
            platform = GetDefaultPlatform();
            status = g_realDispatchTable.GetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 0, nullptr, &nDevicesDetected);
        }

        if (CL_SUCCESS == status)
        {
            cl_device_id* pDevices = nullptr;
            pDevices = new(std::nothrow) cl_device_id[nDevicesDetected];

            if (nullptr != pDevices)
            {
                status = g_realDispatchTable.GetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, nDevicesDetected, pDevices, nullptr);

                if (CL_SUCCESS == status)
                {
                    for (unsigned int i = 0; i < nDevicesDetected; ++i)
                    {
                        status = g_realDispatchTable.GetDeviceInfo(pDevices[i], CL_DEVICE_NAME, MAX_INFO_BUFFER_SIZE, szInfoBuffer, &nBytesRead);

                        if (CL_SUCCESS == status)
                        {
                            platformInfo.m_deviceName.assign(szInfoBuffer);
                        }

                        strcpy(szInfoBuffer, "");
                        status = g_realDispatchTable.GetDeviceInfo(pDevices[i], CL_DEVICE_PLATFORM, sizeof(cl_platform_id), &platform, &nBytesRead);

                        if (CL_SUCCESS == status)
                        {
                            status |= g_realDispatchTable.GetPlatformInfo(platform, CL_PLATFORM_NAME, MAX_INFO_BUFFER_SIZE, szInfoBuffer, &nBytesRead);
                        }

                        if (CL_SUCCESS == status)
                        {
                            platformInfo.m_platformName.assign(szInfoBuffer);
                        }

                        strcpy(szInfoBuffer, "");
                        status |= g_realDispatchTable.GetPlatformInfo(platform, CL_PLATFORM_VERSION, MAX_INFO_BUFFER_SIZE, szInfoBuffer, &nBytesRead);

                        if (CL_SUCCESS == status)
                        {
                            platformInfo.m_platformVersion.assign(szInfoBuffer);
                        }

                        strcpy(szInfoBuffer, "");
                        status |= g_realDispatchTable.GetPlatformInfo(platform, CL_PLATFORM_VENDOR, MAX_INFO_BUFFER_SIZE, szInfoBuffer, &nBytesRead);

                        if (CL_SUCCESS == status)
                        {
                            platformInfo.m_platformVendor.assign(szInfoBuffer);
                        }

                        strcpy(szInfoBuffer, "");
                        status |= g_realDispatchTable.GetDeviceInfo(pDevices[i], CL_DRIVER_VERSION, MAX_INFO_BUFFER_SIZE, szInfoBuffer, &nBytesRead);

                        if (CL_SUCCESS == status)
                        {
                            platformInfo.m_driverVersion.assign(szInfoBuffer);
                        }

                        strcpy(szInfoBuffer, "");
                        status |= g_realDispatchTable.GetDeviceInfo(pDevices[i], CL_DEVICE_VERSION, MAX_INFO_BUFFER_SIZE, szInfoBuffer, &nBytesRead);

                        if (CL_SUCCESS == status)
                        {
                            platformInfo.m_runtimeVersion.assign(szInfoBuffer);
                        }

                        strcpy(szInfoBuffer, "");
                        status |= g_realDispatchTable.GetDeviceInfo(pDevices[i], CL_DEVICE_ADDRESS_BITS, sizeof(cl_uint), &nDeviceAddressBits, &nBytesRead);

                        if (CL_SUCCESS == status)
                        {
                            platformInfo.m_addressBits = nDeviceAddressBits;
                        }

                        strcpy(szInfoBuffer, "");
                        status |= g_realDispatchTable.GetDeviceInfo(pDevices[i], CL_DEVICE_BOARD_NAME_AMD, MAX_INFO_BUFFER_SIZE, szInfoBuffer, &nBytesRead);

                        if (CL_SUCCESS == status)
                        {
                            platformInfo.m_boardName.assign(szInfoBuffer);
                        }

                        platformInfo.m_pcieDeviceId = 0;
                        cl_uint pcieDeviceId = 0;
                        cl_int pcieStatus = g_realDispatchTable.GetDeviceInfo(pDevices[i], CL_DEVICE_PCIE_ID_AMD, sizeof(cl_uint), &pcieDeviceId, nullptr);

                        if (CL_SUCCESS == pcieStatus && 0 != pcieDeviceId)
                        {
                            GDT_HW_GENERATION gen = GDT_HW_GENERATION_NONE;

                            if (AMDTDeviceInfoUtils::Instance()->GetHardwareGeneration(pcieDeviceId, gen))
                            {
                                platformInfo.m_pcieDeviceId = pcieDeviceId;
                            }
                        }

                        if (CL_SUCCESS == status)
                        {
                            platformList.insert(platformInfo);
                        }
                    }
                }

                delete[] pDevices;
                bRetVal = platformList.size() > 0;
            }
        }
    }

    return bRetVal;
}

cl_platform_id CLUtils::GetDefaultPlatform()
{
    std::lock_guard<std::mutex> lock(gs_platformMutex);

    if (INVALID_CL_PLATFORM == gs_defaultPlatform)
    {
        cl_uint numPlatforms;

        // If we haven't already figured out the default platform (which may have been done in
        // AddPlatform), then we have to query the OCL runtime for available platforms.
        //
        // Calling GetPlatformIDs here may result in two clGetPlatformInfo calls appearing
        // in an API trace, if GetDefaultPlatform is called by another agent. This is because
        // the clGetPlatformIDs implementation in opencl.dll/libopencl.so calls clGetPlatformInfo
        // twice to query the ICD suffix. In the case where GetDefaultPlatform is called by an
        // agent, it's possible to see these two calls in the api trace.
        //
        // To avoid this, CLUtils now has a mechanism whereby OpenCL profiler agents can now
        // inform it of any OpenCL platforms they encounter (see the AddPlatform function).
        // This allows CLUtils to keep track of the default (i.e. AMD) platform and always use
        // that without needing to query available platforms. Because of this, the code here to
        // query platforms is now really only used when this code is called from somewhere other
        // than in the context of an agent (for instance when it is called by rcprof.exe while
        // writing an .atp file).

        cl_int status = g_realDispatchTable.GetPlatformIDs(0, NULL, &numPlatforms);

        if (CL_SUCCESS == status && 0 < numPlatforms)
        {
            cl_platform_id* pPlatforms = new(std::nothrow) cl_platform_id[numPlatforms];

            if (nullptr != pPlatforms)
            {
                status = g_realDispatchTable.GetPlatformIDs(numPlatforms, pPlatforms, NULL);

                if (CL_SUCCESS == status)
                {
                    for (unsigned i = 0; i < numPlatforms; ++i)
                    {
                        char pbuf[100] = "";
                        status = g_realDispatchTable.GetPlatformInfo(pPlatforms[i],
                                                                     CL_PLATFORM_VENDOR,
                                                                     sizeof(pbuf),
                                                                     pbuf,
                                                                     NULL);

                        if (!strcmp(pbuf, "Advanced Micro Devices, Inc."))
                        {
                            gs_defaultPlatform = pPlatforms[i];
                            break;
                        }
                    }
                }

                delete[] pPlatforms;
            }
        }
    }

    if (INVALID_CL_PLATFORM == gs_defaultPlatform)
    {
        return nullptr;
    }
    else
    {
        return gs_defaultPlatform;
    }
}

bool CLUtils::QueryKernelInfo(cl_kernel kernel,
                              const std::string& strDeviceName,
                              cl_device_id device,
                              KernelInfo& outKernelInfo)
{
    bool bRetVal = true;

    if (nullptr == clExtAMDDispatchTable::Instance()->GetKernelInfoAMD)
    {
        GPULogger::Log(GPULogger::logERROR, "CLUtils::QueryKernelInfo: GetKernelInfoAMD entry point not initialized\n");
        bRetVal = false;
    }
    else
    {
        size_t tmp;

        cl_int status;

        status = clExtAMDDispatchTable::Instance()->GetKernelInfoAMD(kernel, device, CL_KERNELINFO_SCRATCH_REGS, sizeof(tmp), &tmp, NULL);

        if (CL_SUCCESS == status)
        {
            outKernelInfo.m_nScratchReg = tmp;
        }
        else
        {
            bRetVal = false;
            GPULogger::Log(GPULogger::logERROR, "CLUtils::QueryKernelInfo: GetKernelInfoAMD(CL_KERNELINFO_SCRATCH_REGS) failed\n");
        }

        status = clExtAMDDispatchTable::Instance()->GetKernelInfoAMD(kernel, device, CL_KERNELINFO_WAVEFRONT_SIZE, sizeof(tmp), &tmp, NULL);

        if (CL_SUCCESS == status)
        {
            outKernelInfo.m_nWavefrontSize = tmp;
        }
        else
        {
            bRetVal = false;
            GPULogger::Log(GPULogger::logERROR, "CLUtils::QueryKernelInfo: GetKernelInfoAMD(CL_KERNELINFO_WAVEFRONT_SIZE) failed\n");
        }

        status = clExtAMDDispatchTable::Instance()->GetKernelInfoAMD(kernel, device, CL_KERNELINFO_AVAILABLE_VGPRS, sizeof(tmp), &tmp, NULL);

        if (CL_SUCCESS == status)
        {
            outKernelInfo.m_nAvailableGPRs = tmp;
        }
        else
        {
            bRetVal = false;
            GPULogger::Log(GPULogger::logERROR, "CLUtils::QueryKernelInfo: GetKernelInfoAMD(CL_KERNELINFO_AVAILABLE_VGPRS) failed\n");
        }

        status = clExtAMDDispatchTable::Instance()->GetKernelInfoAMD(kernel, device, CL_KERNELINFO_USED_VGPRS, sizeof(tmp), &tmp, NULL);

        if (CL_SUCCESS == status)
        {
            outKernelInfo.m_nUsedGPRs = tmp;
        }
        else
        {
            bRetVal = false;
            GPULogger::Log(GPULogger::logERROR, "CLUtils::QueryKernelInfo: GetKernelInfoAMD(CL_KERNELINFO_USED_VGPRS) failed\n");
        }

        status = clExtAMDDispatchTable::Instance()->GetKernelInfoAMD(kernel, device, CL_KERNELINFO_AVAILABLE_SGPRS, sizeof(tmp), &tmp, NULL);

        if (CL_SUCCESS == status)
        {
            outKernelInfo.m_nAvailableScalarGPRs = tmp;
        }
        else
        {
            bRetVal = false;
            GPULogger::Log(GPULogger::logERROR, "CLUtils::QueryKernelInfo: GetKernelInfoAMD(CL_KERNELINFO_AVAILABLE_SGPRS) failed\n");
        }

        status = clExtAMDDispatchTable::Instance()->GetKernelInfoAMD(kernel, device, CL_KERNELINFO_USED_SGPRS, sizeof(tmp), &tmp, NULL);

        if (CL_SUCCESS == status)
        {
            outKernelInfo.m_nUsedScalarGPRs = tmp;
        }
        else
        {
            bRetVal = false;
            GPULogger::Log(GPULogger::logERROR, "CLUtils::QueryKernelInfo: GetKernelInfoAMD(CL_KERNELINFO_USED_SGPRS) failed\n");
        }

        status = g_realDispatchTable.GetDeviceInfo(device, CL_DEVICE_LOCAL_MEM_SIZE_PER_COMPUTE_UNIT_AMD, sizeof(tmp), &tmp, NULL);

        if (CL_SUCCESS == status)
        {
            outKernelInfo.m_nAvailableLDSSize = tmp;
        }
        else
        {
            bRetVal = false;
            GPULogger::Log(GPULogger::logERROR, "CLUtils::QueryKernelInfo: GetDeviceInfo(CL_DEVICE_LOCAL_MEM_SIZE_PER_COMPUTE_UNIT_AMD) failed\n");
        }

        // get the used local memory by the kernel
        cl_ulong ulLocalMem = 0;

        status = g_realDispatchTable.GetKernelWorkGroupInfo(kernel, device,
                                                            CL_KERNEL_LOCAL_MEM_SIZE,
                                                            sizeof(cl_ulong), &ulLocalMem, NULL);

        if (CL_SUCCESS == status)
        {
            outKernelInfo.m_nUsedLDSSize = (unsigned long) ulLocalMem;
        }
        else
        {
            bRetVal = false;
            GPULogger::Log(GPULogger::logERROR, "CLUtils::QueryKernelInfo: GetKernelWorkGroupInfo(CL_KERNEL_LOCAL_MEM_SIZE) failed\n");
        }

        SP_TODO("Replace clGetKernelWorkGroupInfo with internal extension GetKernelInfoAMD when this extension is fixed")

        //   if (CL_SUCCESS == clExtAMDDispatchTable::Instance()->GetKernelInfoAMD(kernel, device, CL_KERNELINFO_USED_LDS_SIZE, sizeof(tmp), &tmp, NULL))
        //   {
        //      outKernelInfo.m_nUsedLDSSize = tmp;
        //   }

        GDT_DeviceInfo devInfo;
        cl_uint pcieID;
        cl_int clStatus;

        clStatus = g_realDispatchTable.GetDeviceInfo(device, CL_DEVICE_PCIE_ID_AMD, sizeof(cl_uint), &pcieID, nullptr);

        bool validPcieID = 0 != pcieID;

        if (validPcieID)
        {
            GDT_HW_GENERATION gen = GDT_HW_GENERATION_NONE;

            validPcieID = AMDTDeviceInfoUtils::Instance()->GetHardwareGeneration(pcieID, gen);
        }

        if (CL_SUCCESS != clStatus || !validPcieID)
        {
            if (AMDTDeviceInfoUtils::Instance()->GetDeviceInfo(strDeviceName.c_str(), devInfo))
            {
                outKernelInfo.m_nWavefrontPerSIMD = devInfo.m_nMaxWavePerSIMD;
            }
            else
            {
                size_t nBytesRead = 0;
                char boardName[512];
                cl_int result = g_realDispatchTable.GetDeviceInfo(device, CL_DEVICE_BOARD_NAME_AMD, sizeof(boardName), boardName, &nBytesRead);

                if (CL_SUCCESS == result)
                {
                    std::vector<GDT_GfxCardInfo> cardList;

                    if (AMDTDeviceInfoUtils::Instance()->GetDeviceInfoMarketingName(boardName, cardList))
                    {
                        GDT_HW_ASIC_TYPE asicType = cardList[0].m_asicType;
                        bRetVal = true;

                        for (GDT_GfxCardInfo card : cardList)
                        {
                            if (asicType != card.m_asicType)
                            {
                                bRetVal = false;
                                GPULogger::Log(GPULogger::logERROR, "CLUtils::QueryKernelInfo: GetDeviceInfo failed\n");
                                break;
                            }
                        }

                        if (bRetVal)
                        {
                            if (AMDTDeviceInfoUtils::Instance()->GetDeviceInfo(boardName, devInfo))
                            {
                                outKernelInfo.m_nWavefrontPerSIMD = devInfo.m_nMaxWavePerSIMD;
                            }
                            else
                            {
                                bRetVal = false;
                                GPULogger::Log(GPULogger::logERROR, "CLUtils::QueryKernelInfo: GetDeviceInfo failed\n");
                            }
                        }
                    }
                    else
                    {
                        bRetVal = false;
                        GPULogger::Log(GPULogger::logERROR, "CLUtils::QueryKernelInfo: GetDeviceInfo failed\n");
                    }
                }
                else
                {
                    bRetVal = false;
                    GPULogger::Log(GPULogger::logERROR, "CLUtils::QueryKernelInfo: GetDeviceInfo failed\n");
                }
            }
        }
        else
        {
            if (AMDTDeviceInfoUtils::Instance()->GetDeviceInfo(static_cast<size_t>(pcieID), REVISION_ID_ANY, devInfo))
            {
                outKernelInfo.m_nWavefrontPerSIMD = devInfo.m_nMaxWavePerSIMD;
            }
            else
            {
                bRetVal = false;
                GPULogger::Log(GPULogger::logERROR, "CLUtils::QueryKernelInfo: GetDeviceInfo failed\n");
            }
        }
    }

    return bRetVal;
}

bool CLUtils::EnableQueueProfiling(const cl_queue_properties* properties, QueuePropertiesList& vecProperties)
{
    bool bQueuePropsFound = false;
    bool bUserSetProfileFlag = false;

    if (nullptr != properties)
    {
        // properties is 0 terminated
        while (0 != properties[0])
        {
            vecProperties.push_back(properties[0]);
            bQueuePropsFound = CL_QUEUE_PROPERTIES == properties[0];

            if (bQueuePropsFound)
            {
                properties++;

                cl_command_queue_properties props = properties[0];

                if (0 == (props & CL_QUEUE_PROFILING_ENABLE))
                {
                    props |= CL_QUEUE_PROFILING_ENABLE;
                }
                else
                {
                    bUserSetProfileFlag = true;
                }

                vecProperties.push_back((cl_queue_properties)props);
            }

            if (CL_QUEUE_SIZE == properties[0])
            {
                properties++;
                vecProperties.push_back((cl_queue_properties)properties[0]);
            }

            properties++;
        }
    }

    if (!bQueuePropsFound)
    {
        cl_command_queue_properties props = CL_QUEUE_PROFILING_ENABLE;
        vecProperties.insert(vecProperties.begin(), (cl_queue_properties)props);
        vecProperties.insert(vecProperties.begin(), CL_QUEUE_PROPERTIES);
    }

    vecProperties.push_back(0);

    return bUserSetProfileFlag;
}

void CLUtils::AddPlatform(cl_platform_id platform)
{
    std::lock_guard<std::mutex> lock(gs_platformMutex);

    if (INVALID_CL_PLATFORM == gs_defaultPlatform)
    {
        char pbuf[100] = "";
        cl_int status = g_realDispatchTable.GetPlatformInfo(platform,
                                                            CL_PLATFORM_VENDOR,
                                                            sizeof(pbuf),
                                                            pbuf,
                                                            nullptr);

        if (CL_SUCCESS == status && !strcmp(pbuf, "Advanced Micro Devices, Inc."))
        {
            gs_defaultPlatform = platform;
        }
    }
}
