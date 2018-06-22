//==============================================================================
// Copyright (c) 2015 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief This file defines the structure used to store platform info.
//==============================================================================

#ifndef _CL_PLATFORM_INFO_H_
#define _CL_PLATFORM_INFO_H_

#include <CL/opencl.h>
#include <string>
#include <set>

namespace CLPlatformInfo
{

/// Structure containing the CL platform information
typedef struct PlatformInfo
{
    std::string  m_platformVendor;  ///< Compute platform vendor
    std::string  m_platformName;    ///< Compute platform name
    std::string  m_deviceName;      ///< Compute device name
    std::string  m_platformVersion; ///< Compute platform version string
    std::string  m_driverVersion;   ///< Compute Abstraction Layer version information}
    std::string  m_runtimeVersion;  ///< CL runtime version
    unsigned int m_addressBits;     ///< Number of address bits used by application (application 'bitness')
    std::string  m_boardName;       ///< Compute board name
    cl_uint      m_pcieDeviceId;    ///< PCIE device id
} PlatformInfo;

struct CLPlatformInfoCompare
{
    bool operator()(const CLPlatformInfo::PlatformInfo p1, const CLPlatformInfo::PlatformInfo p2) const;
};

}

typedef std::set<CLPlatformInfo::PlatformInfo, CLPlatformInfo::CLPlatformInfoCompare> CLPlatformSet;

#endif // _CL_PLATFORM_INFO_H_
