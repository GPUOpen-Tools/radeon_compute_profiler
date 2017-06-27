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

const char* PerfMarkerInfoDataHandler::GetPerfMarkerBeginInfoName() const
{
    PerfMarkerBeginEntry* perfMarkerBeginEntry = dynamic_cast<PerfMarkerBeginEntry*>(m_pPerfMarkerEntry);

    if (nullptr != perfMarkerBeginEntry)
    {
        return perfMarkerBeginEntry->m_strName.c_str();
    }

    return nullptr;
}

const char* PerfMarkerInfoDataHandler::GetPerfMarkerBeginInfoGroupName() const
{
    PerfMarkerBeginEntry* perfMarkerBeginEntry = dynamic_cast<PerfMarkerBeginEntry*>(m_pPerfMarkerEntry);

    if (nullptr != perfMarkerBeginEntry)
    {
        return perfMarkerBeginEntry->m_strGroup.c_str();
    }

    return nullptr;
}

const char* PerfMarkerInfoDataHandler::GetPerfMarkerEndExName() const
{
    PerfMarkerEndExEntry* perfMarkerEndExEntry = dynamic_cast<PerfMarkerEndExEntry*>(m_pPerfMarkerEntry);

    if (nullptr != perfMarkerEndExEntry)
    {
        return perfMarkerEndExEntry->m_strName.c_str();
    }

    return nullptr;
}

const char* PerfMarkerInfoDataHandler::GetPerfMarkerEndExGroupName() const
{
    PerfMarkerEndExEntry* perfMarkerEndExEntry = dynamic_cast<PerfMarkerEndExEntry*>(m_pPerfMarkerEntry);

    if (nullptr != perfMarkerEndExEntry)
    {
        return perfMarkerEndExEntry->m_strGroup.c_str();
    }

    return nullptr;
}
