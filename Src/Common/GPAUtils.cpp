//==============================================================================
// Copyright (c) 2015-2018 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief GPUPerfAPI Utilities
//==============================================================================

#include <string.h> //for strcmp
#include <iostream>
#include <algorithm>
#include <functional>
#include <mutex>

#include "OSUtils.h"
#include "GPAUtils.h"
#include "StringUtils.h"
#include "FileUtils.h"
#include "DeviceInfoUtils.h"
#include "Logger.h"
#include "GlobalSettings.h"

using namespace std;
using namespace GPULogger;

std::mutex g_mtx;

GPAApiManager* GPAApiManager::m_pGpaApiManager = nullptr;
GPAFuncTableInfo* g_pFuncTableInfo = NULL;

GPAUtils::GPAUtils()
{
    m_nMaxNumCounter = 1000;
    m_pGetAvailableCountersByGen = nullptr;
    m_pGetAvailableCountersForDevice = nullptr;
    m_api = GPA_API__LAST;
    m_bInit = false;
    m_nMaxPass = GPA_INFINITE_PASS;
    m_contextId = nullptr;
    m_gpaLoaded = false;
}

bool GPAUtils::Open(void* context)
{
    // Remove this lock when GPA is threadsafe
    std::lock_guard<std::mutex> lock(g_mtx);

    if (!m_gpaLoaded)
    {
        return false;
    }

    if (nullptr != m_contextId)
    {
        return false;
    }

    if (StatusCheck(m_gpaFuncTable->GPA_OpenContext(context, GPA_OPENCONTEXT_DEFAULT_BIT, &m_contextId)) != GPA_STATUS_OK)
    {
        return false;
    }

    return true;
}

bool GPAUtils::Close()
{
    if (!m_gpaLoaded)
    {
        return false;
    }

    if (StatusCheck(m_gpaFuncTable->GPA_CloseContext(m_contextId)) != GPA_STATUS_OK)
    {
        return false;
    }

    m_contextId = nullptr;

    return true;
}

bool GPAUtils::EnableCounters(GPA_SessionId sessionId)
{
    if (!m_gpaLoaded)
    {
        return false;
    }

    if (m_selectedCounters.size() == 0)
    {
#ifndef AMDT_INTERNAL
        // enable all counters
        return m_gpaFuncTable->GPA_EnableAllCounters(sessionId) == GPA_STATUS_OK;
#else
        cout << "Unable to enable all counters in internal build." << endl;
        return false;
#endif
    }
    else
    {
        // enable counters from counter file
        if (m_selectedCounters.size() > m_nMaxNumCounter)
        {
            m_selectedCounters.resize(m_nMaxNumCounter);
            Log(logWARNING, "Number of selected counters exceeds the limit(%d), Only the first %d counters are enabled.\n", m_nMaxNumCounter, m_nMaxNumCounter);
        }

        // enable the counter set
        if (!EnableCounterSet(sessionId, m_selectedCounters))
        {
            return false;
        }
    }

    return true;
}


bool GPAUtils::InitGPA(GPA_API_Type api,
                       const gtString& strDLLPath,
                       std::string& strError,
                       const char* pszCounterFile,
                       CounterList* pSelectedCounters,
                       size_t nMaxPass)
{
    m_api = api;
    m_nMaxPass = nMaxPass;

    const char* pszErrorMessage = NULL;

#ifdef _WIN32
    GPA_Status status = GPAApiManager::Instance()->LoadApi(api, strDLLPath.asCharArray());
#else
    string utf8DllPath;
    StringUtils::WideStringToUtf8String(strDLLPath.asCharArray(), utf8DllPath);
    GPA_Status status = GPAApiManager::Instance()->LoadApi(api, utf8DllPath.c_str());
#endif

    if (GPA_STATUS_OK == status)
    {
        m_gpaFuncTable = GPAApiManager::Instance()->GetFunctionTable(m_api);

        if (nullptr == m_gpaFuncTable)
        {
            Log(logERROR, "Unable to get the GPA function table\n");
            return false;
        }

        if (GPA_STATUS_OK != m_gpaFuncTable->GPA_RegisterLoggingCallback(GPA_LOGGING_ERROR_AND_MESSAGE, GPALogCallback))
        {
            Log(logERROR, "Failed to register GPA logging callback\n");
        }

        m_gpaLoaded = true;
    }
    else if (pszErrorMessage != NULL)
    {
        strError = "Unable to load GPUPerfAPI library";
    }

    if (!GPUPerfAPICounterLoader::Instance()->IsLoaded())
    {
        GPUPerfAPICounterLoader::Instance()->LoadPerfAPICounterDll(strDLLPath);
    }

    m_pGetAvailableCountersForDevice = GPUPerfAPICounterLoader::Instance()->GetGPAAvailableCountersForDeviceProc();
    m_pGetAvailableCountersByGen = GPUPerfAPICounterLoader::Instance()->GetGPAAvailableCountersByGenerationProc();

    if (pszCounterFile != NULL)
    {
        CounterList tmpCounterList;
        bool bRet = FileUtils::ReadFile(pszCounterFile, tmpCounterList, true) && GPA_STATUS_OK == status;

        for (CounterList::iterator it = tmpCounterList.begin(); it != tmpCounterList.end(); ++it)
        {
            std::string counterName = StringUtils::Trim(*it); // trim off any whitespace in counter name
            GPA_Hw_Generation gen;
            VerifyCounter(counterName, gen);

            if (gen == GPA_HW_GENERATION_NONE)
            {
                cout << "Unknown counter " << *it << " encountered in the counter file." << endl;
                Log(logTRACE, "Unknown counter %s.\n", it->c_str());
            }
            else
            {
                m_selectedCounters.push_back(*it);
            }
        }

        if (pSelectedCounters != NULL)
        {
            // copy selected counters
            *pSelectedCounters = m_selectedCounters;
        }

        m_bInit = bRet;
        return bRet;
    }
    else
    {
        m_bInit = GPA_STATUS_OK == status;
        return m_bInit;
    }
}

#ifndef _DEBUG
GPA_Status GPAUtils::StatusCheck(GPA_Status status)
{
    return status;
}
#else
GPA_Status GPAUtils::StatusCheck(GPA_Status status)
{
    if (GPA_STATUS_OK > status)
    {
        std::string statusStr = m_gpaFuncTable->GPA_GetStatusAsStr(status);
        statusStr.append("\n");
        Log(logERROR, statusStr.c_str());
    }

    return status;
}
#endif


bool GPAUtils::EnableCounterSet(GPA_SessionId sessionId, const CounterList& selectedCounterNames)
{
    // if the list of selected counters is the full set of counters, and we don't have to worry about max passes,
    // then just call EnableAllCounters.  It is a little faster than using EnableCounterStr, which itself has to loop
    // through the counter list, requesting the counter index and then calling EnableCounter with the index. This gave
    // a minor perf improvement when profiling an application that dispatches many kernels (since for each dispatch,
    // the counters are re-enabled)
    gpa_uint32 numAvailableCounters;

    if (StatusCheck(m_gpaFuncTable->GPA_GetNumCounters(m_contextId, &numAvailableCounters)) == GPA_STATUS_OK)
    {
        if (selectedCounterNames.size() == numAvailableCounters && GPA_INFINITE_PASS == m_nMaxPass)
        {
            if (StatusCheck(m_gpaFuncTable->GPA_EnableAllCounters(sessionId)) == GPA_STATUS_OK)
            {
                return true;
            }
        }
    }

    StatusCheck(m_gpaFuncTable->GPA_DisableAllCounters(sessionId));

    for (size_t i = 0; i < selectedCounterNames.size(); i++)
    {
        if (StatusCheck(m_gpaFuncTable->GPA_EnableCounterByName(sessionId, selectedCounterNames[i].c_str())) != GPA_STATUS_OK)
        {
            Log(logMESSAGE, "Can't enable counter : %s\n", selectedCounterNames[i].c_str());
        }

        if (m_nMaxPass != GPA_INFINITE_PASS)
        {
            gpa_uint32 nPassCount = 0;
            m_gpaFuncTable->GPA_GetPassCount(sessionId, &nPassCount);

            if (nPassCount > m_nMaxPass)
            {
                m_gpaFuncTable->GPA_DisableCounterByName(sessionId, selectedCounterNames[i].c_str());
                cout << "Max number of enabled counters reached. Counter \"" << selectedCounterNames[i] << "\" ignored." << endl;
            }
        }
    }

    return true;
}

GPA_Hw_Generation GPAUtils::GdtHwGenToGpaHwGen(const GDT_HW_GENERATION gdtHwGen)
{
    GPA_Hw_Generation gpaHwGen = GPA_HW_GENERATION_NONE;

    switch (gdtHwGen)
    {
        case GDT_HW_GENERATION_SOUTHERNISLAND:
            gpaHwGen = GPA_HW_GENERATION_SOUTHERNISLAND;
            break;

        case GDT_HW_GENERATION_SEAISLAND:
            gpaHwGen = GPA_HW_GENERATION_SEAISLAND;
            break;

        case GDT_HW_GENERATION_VOLCANICISLAND:
            gpaHwGen = GPA_HW_GENERATION_VOLCANICISLAND;
            break;

        case GDT_HW_GENERATION_GFX9:
            gpaHwGen = GPA_HW_GENERATION_GFX9;
            break;

        default:
            break;
    }

    return gpaHwGen;
}

void GPAUtils::GetCounterValues(GPA_SessionId sessionID, CounterList& counterDataTable, gpa_uint32& sampleCount, gpa_uint32& count)
{
    bool readyResult = false;

    if (sessionID > 0) //TODO: is this check still valid?
    {
        while (!readyResult)
        {
            GPA_Status isSessionReady = m_gpaFuncTable->GPA_IsSessionComplete(sessionID);
            readyResult = GPA_STATUS_OK == isSessionReady;
        }
    }

    m_gpaFuncTable->GPA_GetNumEnabledCounters(sessionID, &count);

    string str;

    m_gpaFuncTable->GPA_GetSampleCount(sessionID, &sampleCount);

    for (gpa_uint32 sample = 0 ; sample < sampleCount ; sample++)
    {
        for (gpa_uint32 counter = 0 ; counter < count ; counter++)
        {
            gpa_uint32 enabledCounterIndex;
            m_gpaFuncTable->GPA_GetEnabledIndex(sessionID, counter, &enabledCounterIndex);

            GPA_Data_Type dataType;

            if (!GetCounterDataType(enabledCounterIndex, dataType))
            {
                Log(logERROR, "Unable to get counter data type.");
            }

            string strName;

            if (!GetCounterName(enabledCounterIndex, strName))
            {
                Log(logERROR, "Unable to get counter name.");
            }

            int precision = 2;

            if (strName.find("GPUTime") != string::npos)
            {
                precision = 5;
            }

            bool bConvertUnit = false;

            /// In GPADX11, TexMemBytesRead is the equivalent counter for FetchSize
            /// execept it's unit is in byte instead of kilobyte
            /// Convert it to kilobyte so that it's consistent in client side
            if (strName.find("TexMemBytesRead") != string::npos)
            {
                bConvertUnit = true;
            }

            /// In GPADX11, FastPath is reported in byte instead of kilobyte
            /// Convert it to kilobyte so that it's consistent in client side
            if (strName.find("CSFastPath") != string::npos)
            {
                bConvertUnit = true;
            }

            // Same for CSCompletePath
            if (strName.find("CSCompletePath") != string::npos)
            {
                bConvertUnit = true;
            }

            size_t sampleResultSize = 0;
            m_gpaFuncTable->GPA_GetSampleResultSize(sessionID, sample, &sampleResultSize);

            if (GPA_DATA_TYPE_UINT64 == dataType)
            {
                gpa_uint64 value;
                m_gpaFuncTable->GPA_GetSampleResult(sessionID, sample, sampleResultSize, &value);

                str = StringUtils::ToString(value);
                counterDataTable.push_back(str);
            }
            else if (GPA_DATA_TYPE_FLOAT64 == dataType)
            {
                gpa_float64 value;
                m_gpaFuncTable->GPA_GetSampleResult(sessionID, sample, sampleResultSize, &value);

                str = StringUtils::ToString(value);
                counterDataTable.push_back(str);

                if (bConvertUnit)
                {
                    value /= 1024.0f;
                }

                str = StringUtils::ToStringPrecision(value, precision);
                counterDataTable.push_back(str);
            }
            else
            {
                Log(logERROR, "Unknown counter type\n");
            }
        }
    }

}

CounterList& GPAUtils::GetCounters(GPA_Hw_Generation generation, const bool shouldIncludeCounterDescriptions)
{
    CounterList& list = m_HWCounterMap[generation];

    if (list.empty())
    {
        IGPACounterAccessor* pAccessor = NULL;
        SpAssertRet(m_pGetAvailableCountersByGen != NULL) list;
        SpAssertRet(m_api != GPA_API__LAST) list;
        m_pGetAvailableCountersByGen(m_api, generation, GPA_OPENCONTEXT_DEFAULT_BIT, true, &pAccessor);

        if (pAccessor == NULL)
        {
            return list;
        }

        gpa_uint32 nCounters = pAccessor->GetNumCounters();
        list.resize(shouldIncludeCounterDescriptions ? nCounters * 3 : nCounters);

        unsigned int counterListIndex = 0;

        for (gpa_uint32 i = 0; i < nCounters; ++i)
        {
            list[counterListIndex++] = pAccessor->GetCounterName(i);

            if (shouldIncludeCounterDescriptions)
            {
                list[counterListIndex++] = pAccessor->GetCounterGroup(i);
                list[counterListIndex++] = pAccessor->GetCounterDescription(i);
            }
        }
    }

    return list;
}

CounterList& GPAUtils::GetCountersForDevice(gpa_uint32 uDeviceid, gpa_uint32 uRevisionid, size_t nMaxPass)
{
    CounterList& list = m_HWCounterDeviceMap[uDeviceid];

    if (list.empty())
    {
        IGPACounterAccessor* pAccessor = NULL;
        IGPACounterScheduler* pScheduler = NULL;
        SpAssertRet(m_pGetAvailableCountersForDevice != NULL) list;
        SpAssertRet(m_api != GPA_API__LAST) list;
        static const int AMD_VENDOR_ID = 0x1002;
        m_pGetAvailableCountersForDevice(m_api, AMD_VENDOR_ID, uDeviceid, uRevisionid, GPA_OPENCONTEXT_DEFAULT_BIT, true, &pAccessor, &pScheduler);
        SpAssertRet(pAccessor != NULL) list;
        SpAssertRet(pScheduler != NULL) list;

        pScheduler->SetCounterAccessor(pAccessor, AMD_VENDOR_ID, uDeviceid, uRevisionid);
        pScheduler->DisableAllCounters();

        gpa_uint32 nCounters = pAccessor->GetNumCounters();
        list.clear();

        gpa_uint32 uRequiredPass;

        for (gpa_uint32 i = 0; i < nCounters; ++i)
        {
            pScheduler->EnableCounter(i);
            pScheduler->GetNumRequiredPasses(&uRequiredPass);

            if (uRequiredPass <= nMaxPass)
            {
                list.push_back(pAccessor->GetCounterName(i));
            }
            else
            {
                pScheduler->DisableCounter(i);
                continue;
            }
        }
    }

    return list;
}

CounterList& GPAUtils::GetCountersForDevice(gpa_uint32 uDeviceid, gpa_uint32 uRevisionid, const bool shouldIncludeCounterDescriptions)
{
    CounterList& list = m_HWCounterDeviceMap[uDeviceid];

    if (list.empty())
    {
        IGPACounterAccessor* pAccessor = NULL;
        IGPACounterScheduler* pScheduler = NULL;
        SpAssertRet(m_pGetAvailableCountersForDevice != NULL) list;
        SpAssertRet(m_api != GPA_API__LAST) list;
        static const int AMD_VENDOR_ID = 0x1002;
        m_pGetAvailableCountersForDevice(m_api, AMD_VENDOR_ID, uDeviceid, uRevisionid, GPA_OPENCONTEXT_DEFAULT_BIT, true, &pAccessor, &pScheduler);
        SpAssertRet(pAccessor != NULL) list;
        SpAssertRet(pScheduler != NULL) list;

        gpa_uint32 nCounters = pAccessor->GetNumCounters();
        list.resize(shouldIncludeCounterDescriptions ? nCounters * 3 : nCounters);

        unsigned int counterListIndex = 0;

        for (gpa_uint32 i = 0; i < nCounters; ++i)
        {
            list[counterListIndex++] = pAccessor->GetCounterName(i);

            if (shouldIncludeCounterDescriptions)
            {
                list[counterListIndex++] = pAccessor->GetCounterGroup(i);
                list[counterListIndex++] = pAccessor->GetCounterDescription(i);
            }
        }
    }

    return list;
}

bool GPAUtils::SetEnabledCounters(const CounterList& countersToEnable)
{
    m_selectedCounters.assign(countersToEnable.begin(), countersToEnable.end());
    return true;
}

bool GPAUtils::GetAvailableCounters(GPA_Hw_Generation generation, CounterList& availableCounters, const bool shouldIncludeCounterDescriptions)
{
    bool retVal = true;

    switch (generation)
    {
        case GPA_HW_GENERATION_SOUTHERNISLAND:
        case GPA_HW_GENERATION_SEAISLAND:
        case GPA_HW_GENERATION_VOLCANICISLAND:
        case GPA_HW_GENERATION_GFX9:
            retVal = true;
            break;

        default:
            retVal = false;
            break;
    }

    if (retVal)
    {
        availableCounters = GetCounters(generation, shouldIncludeCounterDescriptions);
    }

    return retVal;
}

bool GPAUtils::GetAvailableCountersGdt(GDT_HW_GENERATION generation, CounterList& availableCounters)
{
    GPA_Hw_Generation gpaHwGen = GdtHwGenToGpaHwGen(generation);
    bool retVal = true;

    if (GPA_HW_GENERATION_NONE == gpaHwGen)
    {
        retVal = false;
    }
    else
    {
        retVal = GetAvailableCounters(gpaHwGen, availableCounters);
    }

    return retVal;
}

bool GPAUtils::GetAvailableCountersForDevice(gpa_uint32 deviceId, gpa_uint32 revisionId, size_t nMaxPass, CounterList& availableCounters)
{
    availableCounters = GetCountersForDevice(deviceId, revisionId, nMaxPass);
    return true;
}

bool GPAUtils::GetAvailableCountersForDeviceWithoutMaxPass(gpa_uint32 deviceId, gpa_uint32 revisionId, CounterList& availableCounters, const bool shouldIncludeCounterDescriptions)
{
    availableCounters = GetCountersForDevice(deviceId, revisionId, shouldIncludeCounterDescriptions);
    return true;
}

void GPAUtils::VerifyCounter(const std::string& strCounter, GPA_Hw_Generation& generation)
{
    CounterList& list = GetCounters(GPA_HW_GENERATION_GFX9);
    CounterList::iterator it = std::find(list.begin(), list.end(), strCounter);

    if (it != list.end())
    {
        generation = GPA_HW_GENERATION_GFX9;
        return;
    }

    list = GetCounters(GPA_HW_GENERATION_VOLCANICISLAND);
    it = std::find(list.begin(), list.end(), strCounter);

    if (it != list.end())
    {
        generation = GPA_HW_GENERATION_VOLCANICISLAND;
        return;
    }

    list = GetCounters(GPA_HW_GENERATION_SEAISLAND);
    it = std::find(list.begin(), list.end(), strCounter);

    if (it != list.end())
    {
        generation = GPA_HW_GENERATION_SEAISLAND;
        return;
    }

    list = GetCounters(GPA_HW_GENERATION_SOUTHERNISLAND);
    it = std::find(list.begin(), list.end(), strCounter);

    if (it != list.end())
    {
        generation = GPA_HW_GENERATION_SOUTHERNISLAND;
        return;
    }

    generation = GPA_HW_GENERATION_NONE;
}

bool GPAUtils::GetCounterDataType(gpa_uint32 counterIndex, GPA_Data_Type& counterDataType) const
{
    bool retVal = GPA_STATUS_OK == m_gpaFuncTable->GPA_GetCounterDataType(m_contextId, counterIndex, &counterDataType);

    return retVal;
}

bool GPAUtils::GetCounterName(gpa_uint32 counterIndex, std::string& counterName) const
{
    const char* name;
    bool retVal = GPA_STATUS_OK == m_gpaFuncTable->GPA_GetCounterName(m_contextId, counterIndex, &name);

    if (retVal)
    {
        counterName = name;
    }

    return retVal;
}

bool GPAUtils::CreateSession(GPA_SessionId& sessionId) const
{
    bool retVal = GPA_STATUS_OK == m_gpaFuncTable->GPA_CreateSession(m_contextId, GPA_SESSION_SAMPLE_TYPE_DISCRETE_COUNTER, &sessionId);

    return retVal;
}

void GPAUtils::GPALogCallback(GPA_Logging_Type messageType, const char* message)
{
    LogType logType = logMESSAGE;

    if (messageType == GPA_LOGGING_ERROR)
    {
        logType = logERROR;
    }
    else if (messageType == GPA_LOGGING_TRACE)
    {
        logType = logTRACE;
    }

    Log(logType, "GPA: %s\n", message);
}
