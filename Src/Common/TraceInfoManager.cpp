//==============================================================================
// Copyright (c) 2015 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief This class manages ITraceEntry, provides thread-safe API to add and
///        flush trace entries. It's the base class for APIInfoManager, PerfMarkerInfoManager
///        and EventInfoManager (and others)
//==============================================================================

#include "TraceInfoManager.h"
#include <fstream>
#include <ostream>
#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>
#include <mutex>
#include <math.h>

#include <AMDTOSWrappers/Include/osThread.h>

#include "Defs.h"
#include "FileUtils.h"
#include "GlobalSettings.h"
#include "Logger.h"
#include "Version.h"
#include "StringUtils.h"
#include "OSUtils.h"
#include "StackTracer.h"
#include "ProfilerOutputFileDefs.h"

using namespace std;

std::string ITraceEntry::s_strParamSeparator = ATP_TRACE_ENTRY_ARG_SEPARATOR;

TraceInfoManager::TraceInfoManager(void) :
    m_iActiveMap(0),
    m_bTimeOutMode(false),
    m_bIsRunning(true),
    m_uiInterval(100),
    m_cListSeparator(LocaleSetting::GetListSeparator()),
    m_tidTimer(0),
    m_bStopped(false),
    m_timerFunc(nullptr)
{
}

TraceInfoManager::~TraceInfoManager(void)
{
}

void TraceInfoManager::TrySwapBuffer()
{
    // If nonActive map has pending entries, don't swap buffer
    TraceInfoMap& nonActiveMap = m_TraceInfoMap[ 1 - m_iActiveMap ];

    for (TraceInfoMap::iterator mapIt = nonActiveMap.begin(); mapIt != nonActiveMap.end(); mapIt++)
    {
        if (mapIt->second.size() > 0)
        {
            return;
        }
    }

    m_mtx.lock();
    m_iActiveMap = 1 - m_iActiveMap;
    m_mtx.unlock();
}

bool TraceInfoManager::StartTimer(TimerFunc timerFunc)
{
    m_timerFunc = timerFunc;
    return ResumeTimer();
}

void TraceInfoManager::StopTimer()
{
    std::lock_guard<std::mutex> lock(m_mtxStopResume);

    m_bIsRunning = false;
    OSUtils::Instance()->Join(m_tidTimer);
    m_tidTimer = 0;
}

bool TraceInfoManager::ResumeTimer()
{
    std::lock_guard<std::mutex> lock(m_mtxStopResume);

    m_bIsRunning = true;
    bool retVal = false;

    if (nullptr != m_timerFunc)
    {
        m_tidTimer = OSUtils::Instance()->CreateThread(m_timerFunc, nullptr);

        if (0 != m_tidTimer)
        {
            m_bTimeOutMode = true;
            retVal = true;
        }
    }

    return retVal;
}

void TraceInfoManager::AddTraceInfoEntry(ITraceEntry* en)
{
    // lock access to m_TraceInfoMap
    std::lock_guard<std::mutex> lock(m_mtxTracemap);

    if (m_bStopped)
    {
        SAFE_DELETE(en);
        return;
    }

    TraceInfoMap* activeMap = &m_TraceInfoMap[ 0 ];

    if (m_bTimeOutMode)
    {
        // lock the access to active map
        m_mtx.lock();
        activeMap = &m_TraceInfoMap[ m_iActiveMap ];
        m_mtx.unlock();
    }

    en->m_tid = osGetUniqueCurrentThreadId();

    TraceInfoMap::iterator it = activeMap->find(en->m_tid);

    if (it != activeMap->end())
    {
        it->second.push_back(en);
    }
    else
    {
        std::list<ITraceEntry*> list;
        list.push_back(en);
        activeMap->insert(TraceInfoMapPair(en->m_tid, list));
    }
}

void TraceInfoManager::Release()
{
    for (int i = 0; i < 2; i++)
    {
        for (TraceInfoMap::iterator mapIt = m_TraceInfoMap[i].begin(); mapIt != m_TraceInfoMap[i].end(); mapIt++)
        {
            for (list<ITraceEntry*>::iterator listIt = mapIt->second.begin(); listIt != mapIt->second.end(); listIt++)
            {
                ITraceEntry* item = *listIt;

                SAFE_DELETE(item);
            }
        }
    }

    m_TraceInfoMap[0].clear();
    m_TraceInfoMap[1].clear();
}
