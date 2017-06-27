//==============================================================================
// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  This file contains a class to replace a device id
//==============================================================================

#ifndef _CL_DEVICE_REPLACER_
#define _CL_DEVICE_REPLACER_

#include <CL/opencl.h>
#include <TSingleton.h>
#include <vector>
#include <map>

/// Class for replacing the cl device ids
class CLDeviceReplacer : public TSingleton<CLDeviceReplacer>
{
    friend class TSingleton<CLDeviceReplacer>;

public:

    /// Replaces the CL device ids for the GPU device and specified GPU index
    /// \param[in] platform platform id
    /// \param[in] device_type type of the device
    /// \param[in] num_entries number of entries for the devices to be updated
    /// \param[out] device_list list of the device
    /// \param[out] num_devices number of devices of specified device type
    /// \param[in] gpuIndex index of the GPU
    cl_int ReplaceDeviceIds(
        cl_platform_id    platform,
        cl_device_type    device_type,
        cl_uint           num_entries,
        cl_device_id*     device_list,
        cl_uint*          num_devices,
        unsigned int      gpuIndex,
        cl_uint           originalRetCode);

    /// Destructor
    ~CLDeviceReplacer();

private:
    /// Constructor
    CLDeviceReplacer(): m_bIsGPUIterated(false)
    {}

    std::map<cl_platform_id, std::vector<cl_device_id>> m_clPlatformDeviceIdsMap;   ///< list of the device ids for each platform
    std::vector<cl_device_id*>                          m_CLGPUDeviceIdsList;       ///< list of the device ids for housekeeping
    bool                                                m_bIsGPUIterated;           ///< flag indicating the all the platform and devices has been iterated
};

#endif // _CL_DEVICE_REPLACER_
