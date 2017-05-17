//==============================================================================
// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Perf marker info data handler Implementation
//==============================================================================

#include "PerfMarkerInfoDataHandlerImp.h"

bool PerfMarkerInfoDataHandler::IsBeginPerfMarkerEntry(IPerfMarkerBeginInfoDataHandler** ppPerfMarkerBeginInfoDataHandler)
{
    PerfMarkerBeginEntry* pPerfMarkerBeginEntry = dynamic_cast<PerfMarkerBeginEntry*>(m_pPerfMarkerEntry);

    if (nullptr != pPerfMarkerBeginEntry)
    {
        if (nullptr != ppPerfMarkerBeginInfoDataHandler)
        {
            *ppPerfMarkerBeginInfoDataHandler = dynamic_cast<IPerfMarkerBeginInfoDataHandler*>(this);
        }

        return true;
    }

    if (nullptr != ppPerfMarkerBeginInfoDataHandler)
    {
        *ppPerfMarkerBeginInfoDataHandler = nullptr;
    }

    return false;
}

bool PerfMarkerInfoDataHandler::IsEndPerfMarkerEntry()
{
    return m_pPerfMarkerEntry->m_markerType == PerfMarkerEntry::PerfMarkerType_End ? true : false;
}

bool PerfMarkerInfoDataHandler::IsEndExPerfMarkerEntry(IPerfMarkerEndExInfoDataHandler** ppPerfMarkerEndExInfoDataHandler)
{
    PerfMarkerEndExEntry* pPerfMarkerEndExEntry = dynamic_cast<PerfMarkerEndExEntry*>(m_pPerfMarkerEntry);

    if (nullptr != pPerfMarkerEndExEntry)
    {
        if (nullptr != ppPerfMarkerEndExInfoDataHandler)
        {
            *ppPerfMarkerEndExInfoDataHandler = dynamic_cast<IPerfMarkerEndExInfoDataHandler*>(this);
        }

        return true;
    }

    if (nullptr != ppPerfMarkerEndExInfoDataHandler)
    {
        *ppPerfMarkerEndExInfoDataHandler = nullptr;
    }

    return false;
}

unsigned int PerfMarkerInfoDataHandler::GetPerfMarkerType() const
{
    return m_pPerfMarkerEntry->m_markerType;
}

unsigned long long PerfMarkerInfoDataHandler::GetPerfMarkerTimestamp() const
{
    return m_pPerfMarkerEntry->m_timestamp;
}

osThreadId PerfMarkerInfoDataHandler::GetPerfMarkerThreadId() const
{
    return m_pPerfMarkerEntry->m_tid;
}

std::string PerfMarkerInfoDataHandler::GetPerfMarkerBeginInfoName() const
{
    std::string beginInfoName;
    PerfMarkerBeginEntry* perfMarkerBeginEntry = dynamic_cast<PerfMarkerBeginEntry*>(m_pPerfMarkerEntry);

    if (perfMarkerBeginEntry)
    {
        beginInfoName = perfMarkerBeginEntry->m_strName;
    }

    return beginInfoName;
}

std::string PerfMarkerInfoDataHandler::GetPerfMarkerBeginInfoGroupName() const
{
    std::string beginGroupInfoName;
    PerfMarkerBeginEntry* perfMarkerBeginEntry = dynamic_cast<PerfMarkerBeginEntry*>(m_pPerfMarkerEntry);

    if (perfMarkerBeginEntry)
    {
        beginGroupInfoName = perfMarkerBeginEntry->m_strGroup;
    }

    return beginGroupInfoName;
}

std::string PerfMarkerInfoDataHandler::GetPerfMarkerEndExName() const
{
    std::string beginInfoName;
    PerfMarkerEndExEntry* perfMarkerBeginEntry = dynamic_cast<PerfMarkerEndExEntry*>(m_pPerfMarkerEntry);

    if (perfMarkerBeginEntry)
    {
        beginInfoName = perfMarkerBeginEntry->m_strName;
    }

    return beginInfoName;
}

std::string PerfMarkerInfoDataHandler::GetPerfMarkerEndExGroupName() const
{
    std::string beginGroupInfoName;
    PerfMarkerEndExEntry* perfMarkerBeginEntry = dynamic_cast<PerfMarkerEndExEntry*>(m_pPerfMarkerEntry);

    if (perfMarkerBeginEntry)
    {
        beginGroupInfoName = perfMarkerBeginEntry->m_strGroup;
    }

    return beginGroupInfoName;
}
