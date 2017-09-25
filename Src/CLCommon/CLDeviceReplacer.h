//==============================================================================
// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  This file contain functions to replace CL device ids
//==============================================================================

#ifndef _CL_DEVICE_REPLACER_H_
#define _CL_DEVICE_REPLACER_H_

#include <CL/opencl.h>

/// namespace for replacing the cl device ids functions
namespace CLDeviceReplacer
{
/// Replaces the CL device ids for the GPU device and specified GPU index in clGetDeviceId function
/// \param[in] platform platform id
/// \param[in] device_type type of the device
/// \param[in] num_entries number of entries for the devices to be updated
/// \param[out] device_list list of the device
/// \param[out] num_devices number of devices of specified device type
/// \param[in] gpuIndex index of the GPU
cl_int ReplaceDeviceIdsInclGetDeviceIds(
    cl_platform_id    platform,
    cl_device_type    device_type,
    cl_uint           num_entries,
    cl_device_id*     device_list,
    cl_uint*          num_devices,
    unsigned int      gpuIndex,
    unsigned long long*     startTime = nullptr,
    unsigned long long*     endTime = nullptr);

/// Replaces the CL device ids for the GPU device and specified GPU index in clGetContextInfo function
/// \param[in] context context
/// \param[in] param_name name of the parameter
/// \param[in] param_value_size size in bytes for memory pointed by param_value
/// \param[out] param_value values of the deviceIds
/// \param[out] param_value_size_ret actual size in bytes of data being queried by param_value
/// \param[in] gpuIndex index of the GPU
cl_int ReplaceDeviceIdsInclGetContextInfo(
    cl_context              context,
    cl_context_info         param_name,
    size_t                  param_value_size,
    void*                   param_value,
    size_t*                 param_value_size_ret,
    unsigned int            gpuIndex,
    unsigned long long*     startTime = nullptr,
    unsigned long long*     endTime = nullptr);

/// Checks the platform is AMD platform or not
/// \param[in] platformId platform id
/// \return returns true if platform is AMD otherwise false
bool IsAMDPlatform(cl_platform_id platformId);

/// Returns the AMD GPU at given index
/// \param[in] platformId paltform id
/// \param[in] gpuIndex gpu index
/// \param[out] AMDGpuDevicesCount total count of the AMD GPU devices
/// \return device id of the AMD GPU found at requested index otherwise nullptr
cl_device_id GetAMDGPUDeviceAtIndex(
    cl_platform_id platformId,
    unsigned int gpuIndex,
    unsigned int& AMDGpuDevicesCount);

};

#endif // _CL_DEVICE_REPLACER_H_
