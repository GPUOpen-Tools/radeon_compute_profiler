//==============================================================================
// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Perf marker Api Info Interface header
//==============================================================================

#ifndef _I_PERF_MARKER_INFO_DATA_HANDLER_H_
#define _I_PERF_MARKER_INFO_DATA_HANDLER_H_

// std
#include <string>

// common
#include <AMDTOSWrappers/Include/osOSDefinitions.h>

class IPerfMarkerBeginInfoDataHandler;
class IPerfMarkerEndExInfoDataHandler;

/// Perf marker Api Info Data Handler interface
class IPerfMarkerInfoDataHandler
{
public:
    /// Checks the perf marker entry is begin or not
    /// \param[in,opt] ppPerfMarkerBeginInfoDataHandler begin perfmarker interface pointer
    /// \return true if permarker entry is begin otherwise false
    virtual bool IsBeginPerfMarkerEntry(IPerfMarkerBeginInfoDataHandler** ppPerfMarkerBeginInfoDataHandler = nullptr) = 0;

    /// Checks the perf marker entry is end or not
    /// \return true if permarker entry is end otherwise false
    virtual bool IsEndPerfMarkerEntry() = 0;

    /// Checks the perf marker entry is EndEx or not
    /// \param[in,opt] ppPerfMarkerEndExInfoDataHandler end ex perfmarker interface pointer
    /// \return true if permarker entry is EndEx otherwise false
    virtual bool IsEndExPerfMarkerEntry(IPerfMarkerEndExInfoDataHandler** ppPerfMarkerEndExInfoDataHandler = nullptr) = 0;

    /// Get the perf marker type
    /// \return perf marker type (valid values 0,1,2)
    virtual unsigned int GetPerfMarkerType() const = 0;

    /// Get the perf marker timestamp
    /// \return perf marker timestamp
    virtual unsigned long long GetPerfMarkerTimestamp() const = 0;

    /// Get the thread id of the perf marker
    /// \return thread id
    virtual osThreadId GetPerfMarkerThreadId() const = 0;

    /// Virtual Destructor
    virtual ~IPerfMarkerInfoDataHandler() = default;
};


/// Begin Perf marker Api Info Data Handler interface
class IPerfMarkerBeginInfoDataHandler
{
public:
    /// Get the perf marker name
    /// \return perf marker name
    virtual const char* GetPerfMarkerBeginInfoName() const = 0;

    /// Get the perf marker group name
    /// \return perf marker group name
    virtual const char* GetPerfMarkerBeginInfoGroupName() const = 0;

    /// Virtual Destructor
    virtual ~IPerfMarkerBeginInfoDataHandler() = default;
};


/// EndEx Perf marker Api Info Data Handler interface
class IPerfMarkerEndExInfoDataHandler
{
public:
    /// Get the perf marker name
    /// \return perf marker name
    virtual const char* GetPerfMarkerEndExName() const = 0;

    /// Get the perf marker group name
    /// \return perf marker group name
    virtual const char* GetPerfMarkerEndExGroupName() const = 0;

    /// Virtual Destructor
    virtual ~IPerfMarkerEndExInfoDataHandler() = default;
};

#endif // _I_PERF_MARKER_INFO_DATA_HANDLER_H_
