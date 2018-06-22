//==============================================================================
// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Perf marker info data handler Implementation Header
//==============================================================================

#ifndef _PERF_MARKER_INFO_DATA_HANDLER_IMP_H_
#define _PERF_MARKER_INFO_DATA_HANDLER_IMP_H_

#include "IPerfMarkerInfoDataHandler.h"
#include "../sprofile/PerfMarkerAtpFile.h"

/// Perf marker info data handler class
class PerfMarkerInfoDataHandler : public IPerfMarkerInfoDataHandler,
    public IPerfMarkerBeginInfoDataHandler,
    public IPerfMarkerEndExInfoDataHandler
{
public:

    ///Constructor
    /// \param[in] pPerfMarkerEntry perf marker entry
    PerfMarkerInfoDataHandler(PerfMarkerEntry* pPerfMarkerEntry) : m_pPerfMarkerEntry(pPerfMarkerEntry)
    {
    }

    /// Checks the perf marker entry is begin or not
    /// \param[in,opt] ppPerfMarkerBeginInfoDataHandler begin perfmarker interface pointer
    /// \return true if permarker entry is begin otherwise false
    bool IsBeginPerfMarkerEntry(IPerfMarkerBeginInfoDataHandler** ppPerfMarkerBeginInfoDataHandler = nullptr) override;

    /// Checks the perf marker entry is end or not
    /// \return true if permarker entry is end otherwise false
    bool IsEndPerfMarkerEntry() override;

    /// Checks the perf marker entry is EndEx or not
    /// \param[in,opt] ppPerfMarkerEndExInfoDataHandler end ex perfmarker interface pointer
    /// \return true if permarker entry is EndEx otherwise false
    bool IsEndExPerfMarkerEntry(IPerfMarkerEndExInfoDataHandler** ppPerfMarkerEndExInfoDataHandler = nullptr) override;

    /// Get the perf marker type
    /// \return perf marker type (valid values 0,1,2)
    unsigned int GetPerfMarkerType() const override;

    /// Get the perf marker timestamp
    /// \return perf marker timestamp
    unsigned long long GetPerfMarkerTimestamp() const override;

    /// Get the thread id of the perf marker
    /// \return thread id
    osThreadId GetPerfMarkerThreadId() const override;

    /// Get the perf marker name
    /// \return perf marker name
    const char* GetPerfMarkerBeginInfoName() const override;

    /// Get the perf marker group name
    /// \return perf marker group name
    const char* GetPerfMarkerBeginInfoGroupName() const override;

    /// Get the perf marker name
    /// \return perf marker name
    const char* GetPerfMarkerEndExName() const override;

    /// Get the perf marker group name
    /// \return perf marker group name
    const char* GetPerfMarkerEndExGroupName() const override;

    /// Destructor
    ~PerfMarkerInfoDataHandler() {}

private:
    PerfMarkerEntry*                    m_pPerfMarkerEntry;             ///< pointer to perf marker entry
};

#endif // _PERF_MARKER_INFO_DATA_HANDLER_IMP_H_
