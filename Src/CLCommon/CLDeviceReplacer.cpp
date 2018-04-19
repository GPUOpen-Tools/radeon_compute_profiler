//==============================================================================
// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  This file contains implementation to CLDevice Replacer functions
//==============================================================================

#include <algorithm>
#include <vector>

#include "CLDeviceReplacer.h"
#include "CLFunctionDefs.h"
#include "OSUtils.h"

#define AMD_VENDOR_NAME "Advanced Micro Devices"

cl_int CLDeviceReplacer::ReplaceDeviceIdsInclGetDeviceIds(
    cl_platform_id              platform,
    cl_device_type              device_type,
    cl_uint                     num_entries,
    cl_device_id*               device_list,
    cl_uint*                    num_devices,
    unsigned int                gpuIndex,
    unsigned long long*         startTime,
    unsigned long long*         endTime)
{
    cl_int retCode = CL_SUCCESS;

    if (((device_type & CL_DEVICE_TYPE_GPU) == CL_DEVICE_TYPE_GPU))
    {
        if (device_list == nullptr)
        {
            // Device Count Query
            if (nullptr  == num_devices)
            {
                retCode = CL_INVALID_VALUE;
            }
            else
            {
                if (nullptr != startTime)
                {
                    *startTime = OSUtils::Instance()->GetTimeNanos();
                }

                // First get the actual devices in the system for asked device type
                retCode = g_realDispatchTable.GetDeviceIDs(
                              platform,
                              device_type,
                              0u,
                              nullptr,
                              num_devices);

                if (nullptr != endTime)
                {
                    *endTime = OSUtils::Instance()->GetTimeNanos();
                }

                if (CL_SUCCESS == retCode && IsAMDPlatform(platform))
                {
                    // Find the AMD GPU device for the asked platform at the GPUIndex
                    unsigned int AMDGpuDeviceCount = 0u;
                    cl_device_id amdGPUAtAskedGPUIndex = GetAMDGPUDeviceAtIndex(platform, gpuIndex, AMDGpuDeviceCount);

                    // Remove all of the AMD GPU Devices count
                    *num_devices -= static_cast<cl_uint>(AMDGpuDeviceCount);

                    if (nullptr != amdGPUAtAskedGPUIndex)
                    {
                        // Add only if the GPU Index is within the range of the AMD GPU list count
                        *num_devices += 1;
                    }

                    if (0 == *num_devices)
                    {
                        // We haven't added(found) any device of type device_type
                        retCode = CL_DEVICE_NOT_FOUND;
                    }
                }
            }
        }
        else
        {
            // Query Devices
            if (num_entries == 0)
            {
                retCode = CL_INVALID_VALUE;
            }
            else
            {
                cl_uint numOfActualDevices;
                cl_uint success = g_realDispatchTable.GetDeviceIDs(platform, device_type, 0u, nullptr, &numOfActualDevices);
                retCode = success;

                if (CL_SUCCESS == success)
                {
                    cl_device_id* pActualDevices = new(std::nothrow) cl_device_id[numOfActualDevices];

                    if (nullptr != pActualDevices)
                    {
                        if (nullptr != startTime)
                        {
                            *startTime = OSUtils::Instance()->GetTimeNanos();
                        }

                        // Get the actual devices
                        retCode = g_realDispatchTable.GetDeviceIDs(
                                      platform,
                                      device_type,
                                      numOfActualDevices,
                                      pActualDevices,
                                      num_devices);

                        if (nullptr != endTime)
                        {
                            *endTime = OSUtils::Instance()->GetTimeNanos();
                        }
                    }

                    if (CL_SUCCESS == retCode)
                    {
                        if (IsAMDPlatform(platform))
                        {
                            /*
                             * We need to replace the device in the list only if it is "AMD GPU" by
                             * GPU at the asked gpuIndex in only case of it falling in the range of the AMD GPU devices count
                             * otherwise replace it by nullptr
                             */
                            size_t firstAMDGPUIndex = 0ul;
                            bool firstGPUIndexFound = false;

                            // Find the AMD GPU device for the asked platform at the GPUIndex
                            unsigned int AMDGpuDeviceCount = 0u;
                            cl_device_id amdGPUAtAskedGPUIndex = GetAMDGPUDeviceAtIndex(platform, gpuIndex, AMDGpuDeviceCount);

                            cl_uint requestedDeviceCountIter = 0u;
                            bool addedAnyDeviceToDeviceList = false;

                            memset(device_list, NULL, sizeof(cl_device_id) * num_entries);  // Assign nullptr to all

                            for (size_t i = 0; i < numOfActualDevices; i++)
                            {
                                if (requestedDeviceCountIter < num_entries)
                                {
                                    cl_device_type deviceType;
                                    // Check if the device is GPU or not
                                    success = g_realDispatchTable.GetDeviceInfo(pActualDevices[i], CL_DEVICE_TYPE, sizeof(cl_device_type), &deviceType, nullptr);

                                    if (CL_SUCCESS == success && CL_DEVICE_TYPE_GPU == deviceType)
                                    {
                                        if (!firstGPUIndexFound)
                                        {
                                            firstAMDGPUIndex = i;
                                            firstGPUIndexFound = true;
                                        }

                                        if (nullptr != amdGPUAtAskedGPUIndex &&
                                            requestedDeviceCountIter == firstAMDGPUIndex)
                                        {
                                            requestedDeviceCountIter++;
                                        }

                                        continue;
                                    }

                                    device_list[requestedDeviceCountIter] = pActualDevices[i];

                                    if (!addedAnyDeviceToDeviceList)
                                    {
                                        addedAnyDeviceToDeviceList = true;
                                    }

                                    requestedDeviceCountIter++;
                                }
                            }

                            if (nullptr != num_devices)
                            {
                                *num_devices -= static_cast<cl_uint>(AMDGpuDeviceCount);
                            }

                            if (nullptr != amdGPUAtAskedGPUIndex)
                            {
                                // Add the found GPU
                                device_list[firstAMDGPUIndex] = amdGPUAtAskedGPUIndex;

                                if (nullptr != num_devices)
                                {
                                    *num_devices += 1;
                                }
                            }
                            else
                            {
                                /*
                                 * If we don't have any GPU to return, we need to reorder the device list - nullptr can't be in between
                                 */

                                requestedDeviceCountIter = 0u;

                                while (requestedDeviceCountIter + 1 < num_entries)
                                {
                                    if (nullptr == device_list[requestedDeviceCountIter])
                                    {
                                        device_list[requestedDeviceCountIter] = device_list[requestedDeviceCountIter + 1];
                                        device_list[requestedDeviceCountIter + 1] = nullptr;
                                    }

                                    requestedDeviceCountIter++;
                                }

                                if (!addedAnyDeviceToDeviceList)
                                {
                                    retCode = CL_DEVICE_NOT_FOUND;
                                }
                            }
                        }
                        else
                        {
                            // Non-AMD Platform
                            if (nullptr != startTime)
                            {
                                *startTime = OSUtils::Instance()->GetTimeNanos();
                            }

                            // Call API without any change
                            retCode = g_realDispatchTable.GetDeviceIDs(
                                          platform,
                                          device_type,
                                          num_entries,
                                          device_list,
                                          num_devices);

                            if (nullptr != endTime)
                            {
                                *endTime = OSUtils::Instance()->GetTimeNanos();
                            }
                        }
                    }
                }
            }
        }
    }

    return retCode;
}

cl_int CLDeviceReplacer::ReplaceDeviceIdsInclGetContextInfo(
    cl_context                  context,
    cl_context_info             param_name,
    size_t                      param_value_size,
    void*                       param_value,
    size_t*                     param_value_size_ret,
    unsigned int                gpuIndex,
    unsigned long long*         startTime,
    unsigned long long*         endTime)
{
    auto GetAMDGPUDeviceInDeviceList = [](cl_device_id * deviceId,
                                          size_t deviceCount,
                                          size_t& firstAMDGPUIndex)->std::vector<cl_device_id>
    {
        std::vector<cl_device_id> amdGPUDevices;
        bool firstAMDGPUIndexFound = false;

        for (size_t i = 0; i < deviceCount; i++)
        {
            cl_int success = CL_SUCCESS;
            cl_device_type deviceType = CL_DEVICE_TYPE_DEFAULT;
            success = g_realDispatchTable.GetDeviceInfo(deviceId[i], CL_DEVICE_TYPE, sizeof(cl_device_type), &deviceType, nullptr);

            if (CL_SUCCESS == success && deviceType == CL_DEVICE_TYPE_GPU)
            {
                cl_platform_id gpuPlatform;
                success = g_realDispatchTable.GetDeviceInfo(deviceId[i], CL_DEVICE_PLATFORM, sizeof(cl_platform_id), &gpuPlatform, nullptr);

                if (CL_SUCCESS == success)
                {
                    if (IsAMDPlatform(gpuPlatform))
                    {
                        amdGPUDevices.push_back(deviceId[i]);

                        if (!firstAMDGPUIndexFound)
                        {
                            firstAMDGPUIndex = i;
                            firstAMDGPUIndexFound = true;
                        }
                    }
                }
            }
        }

        return amdGPUDevices;
    };

    cl_int retCode = CL_SUCCESS;

    if (CL_CONTEXT_NUM_DEVICES == param_name)
    {
        cl_uint success = CL_SUCCESS;

        // query context device count
        if (nullptr != startTime)
        {
            *startTime = OSUtils::Instance()->GetTimeNanos();
        }

        success = g_realDispatchTable.GetContextInfo(
                      context,
                      param_name,
                      param_value_size,
                      param_value,
                      param_value_size_ret);

        if (nullptr != endTime)
        {
            *endTime = OSUtils::Instance()->GetTimeNanos();
        }

        if (CL_SUCCESS == success && nullptr != param_value)
        {
            cl_uint contextDeviceCount = *reinterpret_cast<cl_uint*>(param_value);

            if (contextDeviceCount > 0)
            {
                cl_device_id* clDeviceIds = new(std::nothrow) cl_device_id[contextDeviceCount];

                if (nullptr != clDeviceIds)
                {
                    g_realDispatchTable.GetContextInfo(
                        context,
                        CL_CONTEXT_DEVICES,
                        sizeof(cl_device_id) * contextDeviceCount,
                        clDeviceIds,
                        nullptr);

                    size_t firstAMDGPUIndex = 0ul;
                    std::vector<cl_device_id> amdGPUDevices = GetAMDGPUDeviceInDeviceList(clDeviceIds, static_cast<size_t>(contextDeviceCount), firstAMDGPUIndex);

                    // Only need to change the value of "param_value"
                    // As return type for all out parameters except param_value would be sizeof(cl_uint), there won't be any affect on other parameters
                    *reinterpret_cast<cl_uint*>(param_value) = *(reinterpret_cast<cl_uint*>(param_value) - amdGPUDevices.size());

                    if (amdGPUDevices.size() > gpuIndex)
                    {
                        // One Gpu Device
                        *reinterpret_cast<cl_uint*>(param_value) += 1;
                    }

                    if (0 == *reinterpret_cast<cl_uint*>(param_value))
                    {
                        // TO-DO: Don't Create Context at all when there is no gpu at given GPU index

                        /*
                         * If context is created using GPU device (as we don't have control on context creation)
                         * and there is no GPU device - let's return Invalid context so no dispatch of kernel can occur
                         */
                        retCode = CL_SUCCESS;  // Do we need to return CL_INVALID_CONTEXT?
                    }
                }

                delete[] clDeviceIds;
            }
        }

        retCode =  success;
    }

    if (CL_CONTEXT_DEVICES == param_name)
    {
        cl_int success = CL_SUCCESS;

        size_t contextDevicesInBytes;

        if (nullptr != startTime)
        {
            *startTime = OSUtils::Instance()->GetTimeNanos();
        }

        success = g_realDispatchTable.GetContextInfo(
                      context,
                      CL_CONTEXT_DEVICES,
                      0u,
                      nullptr,
                      &contextDevicesInBytes);

        if (nullptr != endTime)
        {
            *endTime = OSUtils::Instance()->GetTimeNanos();
        }

        if (CL_SUCCESS == success)
        {
            size_t contextDeviceCount = contextDevicesInBytes / sizeof(cl_device_id);
            cl_device_id* pActualContextDevices = new(std::nothrow) cl_device_id[contextDeviceCount];

            if (startTime != nullptr && param_value != nullptr)
            {
                *startTime = OSUtils::Instance()->GetTimeNanos();
            }

            /*
             * Note: cl doesn't report any error if param_value_size
             * (i.e. device count in bytes even if they are not multiple of sizeof(cl_device_id))
             * is more than devices used in creating context.
             */

            // Get the actual devices used in creating the context
            success = g_realDispatchTable.GetContextInfo(
                          context,
                          CL_CONTEXT_DEVICES,
                          contextDevicesInBytes,
                          pActualContextDevices,
                          param_value_size_ret);

            if (nullptr != endTime && param_value != nullptr)
            {
                *endTime = OSUtils::Instance()->GetTimeNanos();
            }

            if (CL_SUCCESS == success)
            {
                size_t firstAMDGPUIndex = 0ul;
                bool AMDGPUFoundAtIndex = false;
                /*
                 * We need to find the amd device in the device
                 * list used in creating the context - and not all the AMD Gpu devices
                 */
                std::vector<cl_device_id> amdGPUDevices = GetAMDGPUDeviceInDeviceList(pActualContextDevices, static_cast<size_t>(contextDeviceCount), firstAMDGPUIndex);
                size_t contextDeviceInBytesToReturn = contextDevicesInBytes - (sizeof(cl_device_id) * amdGPUDevices.size());

                if (amdGPUDevices.size() > gpuIndex)
                {
                    contextDeviceInBytesToReturn += sizeof(cl_device_id);
                    AMDGPUFoundAtIndex = true;
                }

                // device list
                if (nullptr != param_value_size_ret)
                {
                    /*
                     * Note: Even if param_value_size is more (in bytes) than used in creating the device
                     * param_value_size_ret always return the actual data transfer in cl runtime
                     */
                    *param_value_size_ret = contextDeviceInBytesToReturn;
                }

                if (nullptr != param_value)
                {
                    size_t devicesToReturn = contextDeviceInBytesToReturn / sizeof(cl_device_id);

                    if (amdGPUDevices.size() > 0)
                    {
                        // Assign nullptr to all - we should not go beyond what we need to return
                        // even param_value_size is greater than param_value_size_ret
                        memset(reinterpret_cast<cl_device_id*>(param_value), NULL, contextDeviceInBytesToReturn);
                        /*
                         * We have AMD GPU device in the context
                         * but need to only return the gpu at gpu index otherwise nullptr
                         */
                        cl_uint requestedDeviceCountIter = 0u;
                        bool addedAnyDeviceToDeviceList = false;

                        for (size_t i = 0; i < contextDeviceCount; i++)
                        {
                            if (requestedDeviceCountIter < devicesToReturn)
                            {
                                cl_device_type deviceType;
                                // Check if the device is GPU or not
                                success = g_realDispatchTable.GetDeviceInfo(pActualContextDevices[i], CL_DEVICE_TYPE, sizeof(cl_device_type), &deviceType, nullptr);

                                if (CL_SUCCESS == success && CL_DEVICE_TYPE_GPU == deviceType)
                                {
                                    if (AMDGPUFoundAtIndex &&
                                        requestedDeviceCountIter == firstAMDGPUIndex)
                                    {
                                        requestedDeviceCountIter++;
                                    }

                                    continue;
                                }

                                reinterpret_cast<cl_device_id*>(param_value)[requestedDeviceCountIter] = pActualContextDevices[i];

                                if (!addedAnyDeviceToDeviceList)
                                {
                                    addedAnyDeviceToDeviceList = true;
                                }

                                requestedDeviceCountIter++;
                            }
                        }

                        if (AMDGPUFoundAtIndex)
                        {
                            // Add the GPU at the first gpu index
                            reinterpret_cast<cl_device_id*>(param_value)[firstAMDGPUIndex] = amdGPUDevices.at(gpuIndex);
                        }
                        else
                        {
                            /*
                            * If we don't have any GPU to return, we need to reorder the device list - nullptr can't be in between
                            */
                            requestedDeviceCountIter = 0u;

                            while (requestedDeviceCountIter + 1 < devicesToReturn)
                            {
                                if (nullptr == reinterpret_cast<cl_device_id*>(param_value)[requestedDeviceCountIter])
                                {
                                    reinterpret_cast<cl_device_id*>(param_value)[requestedDeviceCountIter] = reinterpret_cast<cl_device_id*>(param_value)[requestedDeviceCountIter + 1];
                                    reinterpret_cast<cl_device_id*>(param_value)[requestedDeviceCountIter + 1] = nullptr;
                                }

                                requestedDeviceCountIter++;
                            }

                            if (!addedAnyDeviceToDeviceList)
                            {
                                retCode = CL_DEVICE_NOT_FOUND;
                            }
                        }
                    }
                    else
                    {
                        // There is no AMD GPU Device(s) in the context - return unaltered
                        memcpy(param_value, pActualContextDevices, contextDeviceInBytesToReturn);
                    }
                }
            }

            delete[] pActualContextDevices;
        }

        retCode = success;
    }

    return retCode;
}


bool CLDeviceReplacer::IsAMDPlatform(cl_platform_id platformId)
{
    constexpr size_t vendorNameSize = 256;
    char platformVendorName[vendorNameSize];
    cl_uint success = g_realDispatchTable.GetPlatformInfo(platformId, CL_PLATFORM_VENDOR, vendorNameSize, platformVendorName, nullptr);

    if (CL_SUCCESS == success && (std::string(platformVendorName).find(AMD_VENDOR_NAME) != std::string::npos))
    {
        return true;
    }

    return false;
}

cl_device_id CLDeviceReplacer::GetAMDGPUDeviceAtIndex(cl_platform_id platformId, unsigned int gpuIndex, unsigned int& AMDGpuDevicesCount)
{
    cl_device_id retDeviceId = nullptr;
    AMDGpuDevicesCount = 0u;

    if (IsAMDPlatform(platformId))
    {
        cl_uint gpuDevicesCount;
        cl_int success = g_realDispatchTable.GetDeviceIDs(
                             platformId,
                             CL_DEVICE_TYPE_GPU,
                             0u,
                             nullptr,
                             &gpuDevicesCount);

        if (CL_SUCCESS == success)
        {
            AMDGpuDevicesCount = gpuDevicesCount;

            if (gpuDevicesCount > gpuIndex)
            {
                cl_device_id* deviceIds = new(std::nothrow) cl_device_id[gpuDevicesCount];

                if (nullptr != deviceIds)
                {
                    success = g_realDispatchTable.GetDeviceIDs(
                                  platformId,
                                  CL_DEVICE_TYPE_GPU,
                                  gpuDevicesCount,
                                  deviceIds,
                                  nullptr);

                    retDeviceId = *(deviceIds + gpuIndex);
                }

                delete[]deviceIds;
            }
        }
    }

    return retDeviceId;
}
