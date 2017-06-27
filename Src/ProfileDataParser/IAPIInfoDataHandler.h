//==============================================================================
// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Api Info Interface header
//==============================================================================

#ifndef _I_API_INFO_DATA_HANDLER_H_
#define _I_API_INFO_DATA_HANDLER_H_

// common
#include <AMDTOSWrappers/Include/osOSDefinitions.h>
#include <Defs.h>

/// Api Info data handler interface
class IAPIInfoDataHandler
{
public:
    /// Gets the api start time
    /// \return start time of the api
    virtual ULONGLONG GetApiStartTime() const = 0;

    /// Gets the end time of the api
    /// \return end time of the api
    virtual ULONGLONG GetApiEndTime() const = 0;

    /// Gets the sequence id of the api
    /// \return sequence id of the api
    virtual unsigned int GetApiSequenceId() const = 0;

    /// Gets the displayable sequence id of the api
    /// \return sequence id for the display
    virtual unsigned int GetApiDisplaySequenceId() const = 0;

    /// Checks api available for displaybale sequence or not
    /// \return flag indicating api is displayable sequence or not
    virtual bool IsApiSequenceIdDisplayble() const = 0;

    /// Get the thread id of the api
    /// \return thread id of the api
    virtual osThreadId GetApiThreadId() const = 0;

    /// Get the return type of the api as char array
    /// \return return type of api
    virtual const char* GetApiRetString() const = 0;

    /// Get the api name as string
    /// \return name of the api as char array
    virtual const char* GetApiNameString() const = 0;

    /// Get the argument list of the api as char array
    /// \return argument list of the api as char array
    virtual const char* GetApiArgListString() const = 0;

    /// Virtual Destructor
    virtual ~IAPIInfoDataHandler() = default;
};

#endif // _I_API_INFO_DATA_HANDLER_H_
