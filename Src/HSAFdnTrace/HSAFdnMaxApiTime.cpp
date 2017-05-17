//==============================================================================
// Copyright (c) 2016 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  This file manages the max api call end time - applicable with --maxapicalls option of the profiler
//==============================================================================

#include "HSAFdnMaxApiTime.h"

bool HSAFdnMaxApiCallTime::GetMaxApiCallEndTime(uint64_t& maxApiCallEndTime)
{
    if (m_bEndTimeRecordedForMaxApiCall)
    {
        maxApiCallEndTime = m_maxApiCallEndTime;
    }

    return m_bEndTimeRecordedForMaxApiCall;
}


void HSAFdnMaxApiCallTime::RecordMaxApiCallEndTime(uint64_t maxApiCallEndTime)
{
    if (!m_bEndTimeRecordedForMaxApiCall)
    {
        m_maxApiCallEndTime = maxApiCallEndTime;
        m_bEndTimeRecordedForMaxApiCall = true;
    }
}


HSAFdnMaxApiCallTime::HSAFdnMaxApiCallTime():
    m_maxApiCallEndTime(0ul),
    m_bEndTimeRecordedForMaxApiCall(false)
{ }