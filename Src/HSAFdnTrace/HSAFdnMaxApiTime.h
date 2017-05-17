//==============================================================================
// Copyright (c) 2016 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  This file manages the max api call end time - applicable with --maxapicalls option of the profiler
//==============================================================================

#include "TSingleton.h"
#include <inttypes.h>

#ifndef _HSA_MAX_API_CALL_TIME_H_
#define _HSA_MAX_API_CALL_TIME_H_

class HSAFdnMaxApiCallTime : public TSingleton<HSAFdnMaxApiCallTime>
{
    friend class TSingleton<HSAFdnMaxApiCallTime>;
public:
    /// Get the status of the max api end time record
    /// \param maxApiCallEndTime updates the end time if it is recorded
    /// \return status of the max api call end time record
    bool GetMaxApiCallEndTime(uint64_t& maxApiCallEndTime);

    /// Records the time max api call end time
    /// \param maxApiCallEndTime end time of the max api call
    void RecordMaxApiCallEndTime(uint64_t maxApiCallEndTime);
private:
    /// Constructor
    HSAFdnMaxApiCallTime();

    uint64_t     m_maxApiCallEndTime;               ///< end time of the last api call - used only when maxapicall option is used
    bool         m_bEndTimeRecordedForMaxApiCall;   ///< flag to check the end time after the max api call is recorded
};

#endif