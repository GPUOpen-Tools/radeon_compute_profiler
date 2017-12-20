//==============================================================================
// Copyright (c) 2014 - 2016 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief This class maintains a reference count against a thread id
//==============================================================================

#include "RefTracker.h"
#include <assert.h>
//
/// This class helps detect reentrance problems
//
RefTrackerCounter::RefTrackerCounter()
{
    m_IsUsingExternalMutex = false;
    m_pmutex = new std::mutex();
}

RefTrackerCounter::~RefTrackerCounter()
{
    if (m_pmutex)
    {
        delete m_pmutex;
    }
}

RefTrackerCounter::RefTrackerCounter(std::mutex* pM)
{
    m_IsUsingExternalMutex = true;
    assert(pM);
    m_pmutex = pM;
}

void RefTrackerCounter::UseExternalMutex(std::mutex* pM)
{
    if (m_IsUsingExternalMutex == false)
    {
        delete m_pmutex;
    }

    m_IsUsingExternalMutex = true;

    m_pmutex = pM;


}

//
// Returns a thread's ID, this function helps debugging by, for example forcing the returned value to be always the same
//
static DWORD GetThreadsID()
{
    return GetCurrentThreadId();
}

void RefTrackerCounter::operator++(int)
{
    std::lock_guard<std::mutex> lock(*m_pmutex);

    DWORD dwThreadId = GetThreadsID();
    std::map<DWORD, int>::iterator it = m_mapInsideWrapper.find(dwThreadId);

    if (it != m_mapInsideWrapper.end())
    {
        it->second++;
    }
    else
    {
        m_mapInsideWrapper[dwThreadId] = 1;
    }
}

void RefTrackerCounter::operator--(int)
{
    std::lock_guard<std::mutex> lock(*m_pmutex);

    DWORD dwThreadId = GetThreadsID();
    std::map<DWORD, int>::iterator it = m_mapInsideWrapper.find(dwThreadId);

    if (it != m_mapInsideWrapper.end())
    {
        it->second--;
    }
    else
    {
        //not found? this should be impossible, assert!!
        assert(false);
    }
}

bool RefTrackerCounter::operator==(DWORD v)
{
    std::lock_guard<std::mutex> lock(*m_pmutex);

    DWORD dwThreadId = GetThreadsID();
    std::map<DWORD, int>::iterator it = m_mapInsideWrapper.find(dwThreadId);

    if (it != m_mapInsideWrapper.end())
    {
        return (DWORD)it->second == v;
    }

    //not found? then its value is zero
    return (v == 0);
}

bool RefTrackerCounter::operator>(DWORD v)
{
    std::lock_guard<std::mutex> lock(*m_pmutex);

    DWORD dwThreadId = GetThreadsID();
    std::map<DWORD, int>::iterator it = m_mapInsideWrapper.find(dwThreadId);

    if (it != m_mapInsideWrapper.end())
    {
        return (DWORD)it->second > v;
    }

    return (v == 0);
}

DWORD RefTrackerCounter::GetRef()
{
    std::lock_guard<std::mutex> lock(*m_pmutex);

    DWORD dwThreadId = GetThreadsID();
    std::map<DWORD, int>::iterator it = m_mapInsideWrapper.find(dwThreadId);

    if (it != m_mapInsideWrapper.end())
    {
        return it->second;
    }

    //zero
    return 0;
}

RefTracker::RefTracker(RefTrackerCounter* dwVal)
{
    m_dwVal = dwVal;

    assert(m_dwVal);

    (*m_dwVal)++;
}