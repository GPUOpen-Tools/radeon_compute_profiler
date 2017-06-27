//==============================================================================
// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  This file contains implementation to CLDevice Replacer class
//==============================================================================

#include "CLDeviceReplacer.h"
#include "CLFunctionDefs.h"

cl_int CLDeviceReplacer::ReplaceDeviceIds(cl_platform_id platform, cl_device_type device_type, cl_uint num_entries, cl_device_id* device_list, cl_uint* num_devices, unsigned int gpuIndex, cl_uint originalRetCode)
{
    cl_int ret = originalRetCode;

    if (!m_bIsGPUIterated)
    {
        m_bIsGPUIterated = true;
        cl_uint deviceCount;
        bool success = CL_SUCCESS == (ret = g_realDispatchTable.GetDeviceIDs(nullptr, CL_DEVICE_TYPE_GPU, 0u, nullptr, &deviceCount));

        if (!success)
        {
            cl_uint platformCount;
            success = CL_SUCCESS == (ret = g_realDispatchTable.GetPlatformIDs(0u, nullptr, &platformCount));
            success = CL_SUCCESS == (ret = g_realDispatchTable.GetDeviceIDs(nullptr, CL_DEVICE_TYPE_GPU, 0u, nullptr, &deviceCount));
        }

        if (success && 0u < deviceCount)
        {
            cl_device_id* deviceIds = new(std::nothrow) cl_device_id[deviceCount];
            m_CLGPUDeviceIdsList.push_back(deviceIds);

            if (nullptr != deviceIds)
            {
                success = CL_SUCCESS == (ret = g_realDispatchTable.GetDeviceIDs(nullptr, CL_DEVICE_TYPE_GPU, deviceCount, deviceIds, nullptr));

                if (success)
                {
                    for (unsigned int deviceIndex = 0; deviceIndex < deviceCount; deviceIndex++)
                    {
                        cl_platform_id devicePlatform = nullptr;
                        success = CL_SUCCESS == (ret = g_realDispatchTable.GetDeviceInfo(deviceIds[deviceIndex], CL_DEVICE_PLATFORM, sizeof(cl_platform_id), &devicePlatform, nullptr));

                        if (success && nullptr != devicePlatform)
                        {
                            if (m_clPlatformDeviceIdsMap.find(devicePlatform) == m_clPlatformDeviceIdsMap.end())
                            {
                                std::vector<cl_device_id> deviceIdList;
                                m_clPlatformDeviceIdsMap.insert(std::pair<cl_platform_id, std::vector<cl_device_id>>(devicePlatform, deviceIdList));
                            }

                            m_clPlatformDeviceIdsMap[devicePlatform].push_back(deviceIds[deviceIndex]);
                        }
                    }
                }
            }
        }
    }

    if (0u <= gpuIndex)
    {
        if (CL_DEVICE_TYPE_GPU == (CL_DEVICE_TYPE_GPU & device_type))
        {
            // Check is there any cldevice exist apart from GPUs for specified deviceType
            cl_uint nonGPUDeviceCount = 0u;
            bool success = CL_SUCCESS == (ret = g_realDispatchTable.GetDeviceIDs(platform, (device_type ^ CL_DEVICE_TYPE_GPU), 0u, nullptr, &nonGPUDeviceCount));

            // In case of querying only device Count
            if (nullptr == device_list && nullptr != num_devices)
            {
                cl_uint gpuDeviceCountForThePlatform = static_cast<uint32_t>(m_clPlatformDeviceIdsMap[platform].size());

                if (0u < *num_devices)
                {
                    if (*num_devices >= gpuDeviceCountForThePlatform)
                    {
                        // This contains only GPU devices or GPU Devices along with other devices
                        if (gpuIndex < gpuDeviceCountForThePlatform)
                        {
                            // Only sigle gpu available
                            *num_devices = (*num_devices - gpuDeviceCountForThePlatform) + 1;
                        }
                        else
                        {
                            // Index is greater than the GPU devices - No GPU
                            *num_devices = *num_devices - gpuDeviceCountForThePlatform;
                        }
                    }
                }
                else
                {
                    // If we reached here it means either this function gets called before the actual call or there is no device exist with specified deviceType

                    if (success)
                    {
                        if (0u < m_clPlatformDeviceIdsMap[platform].size() && gpuIndex < static_cast<unsigned int>(m_clPlatformDeviceIdsMap[platform].size()))
                        {
                            // Handle in case Function is called before actual call to the function
                            *num_devices = nonGPUDeviceCount + 1;
                        }
                        else
                        {
                            // No GPU device available - return non GPU device Count
                            *num_devices = nonGPUDeviceCount;
                        }
                    }
                }

                ret = 0 == *num_devices ? CL_DEVICE_NOT_FOUND : CL_SUCCESS;
            }
            else if (nullptr != device_list)
            {
                // Case: when querying for device list (not the count)
                bool isGPUExistForTheSpecifiedPlatformAndIndex = static_cast<uint32_t>(m_clPlatformDeviceIdsMap[platform].size()) > static_cast<uint32_t>(gpuIndex);

                if (isGPUExistForTheSpecifiedPlatformAndIndex)
                {
                    if (nullptr != num_devices)
                    {
                        *num_devices = 1; // Only one GPU is available
                    }

                    if (num_entries > 0u)
                    {
                        for (unsigned int i = 0; i < num_entries; i++)
                        {
                            cl_device_type clCurrentDeviceType;

                            if (CL_SUCCESS == (ret = g_realDispatchTable.GetDeviceInfo(device_list[i], CL_DEVICE_TYPE, sizeof(cl_device_type), &clCurrentDeviceType, nullptr)))
                            {
                                if (CL_DEVICE_TYPE_GPU == clCurrentDeviceType)
                                {
                                    if (!isGPUExistForTheSpecifiedPlatformAndIndex || device_list[i] != m_clPlatformDeviceIdsMap[platform][gpuIndex])
                                    {
                                        device_list[i] = nullptr;
                                    }
                                }
                            }
                        }

                        ret = CL_SUCCESS;
                    }
                    else
                    {
                        ret = originalRetCode;
                    }
                }
                else
                {
                    ret = CL_DEVICE_NOT_FOUND;
                }
            }
        }
        else
        {
            // Not querying for GPU device - Don't do anything
            ret = originalRetCode;
        }
    }

    return ret;
}

CLDeviceReplacer::~CLDeviceReplacer()
{
    for (std::vector<cl_device_id*>::iterator it = m_CLGPUDeviceIdsList.begin(); it != m_CLGPUDeviceIdsList.end(); ++it)
    {
        delete[](*it);
    }
}
