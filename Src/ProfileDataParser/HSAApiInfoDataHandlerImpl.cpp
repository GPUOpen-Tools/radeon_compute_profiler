//==============================================================================
// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief HSAApiInfoDataHandlerImpl Implementation
//==============================================================================

#include "HSAApiInfoDataHandlerImpl.h"

ULONGLONG HSAAPIInfoDataHandler::GetApiStartTime() const
{
    return m_pHsaApiInfo->m_ullStart;
}

ULONGLONG HSAAPIInfoDataHandler::GetApiEndTime() const
{
    return m_pHsaApiInfo->m_ullEnd;
}

unsigned HSAAPIInfoDataHandler::GetApiSequenceId() const
{
    return m_pHsaApiInfo->m_uiSeqID;
}

unsigned HSAAPIInfoDataHandler::GetApiDisplaySequenceId() const
{
    return m_pHsaApiInfo->m_uiDisplaySeqID;
}

bool HSAAPIInfoDataHandler::IsApiSequenceIdDisplayble() const
{
    return m_pHsaApiInfo->m_bHasDisplayableSeqId;
}

osThreadId HSAAPIInfoDataHandler::GetApiThreadId() const
{
    return m_pHsaApiInfo->m_tid;
}

const char* HSAAPIInfoDataHandler::GetApiRetString() const
{
    return m_pHsaApiInfo->m_strRet.c_str();
}

const char* HSAAPIInfoDataHandler::GetApiNameString() const
{
    return m_pHsaApiInfo->m_strName.c_str();
}

const char* HSAAPIInfoDataHandler::GetApiArgListString() const
{
    return m_pHsaApiInfo->m_argList.c_str();
}

bool HSAAPIInfoDataHandler::IsHSAMemoryApi(IHSAMemoryApiInfoDataHandler** ppHsaMemoryApiInfoDataHandler)
{
    HSAMemoryAPIInfo* pHsaMemoryApiInfo = dynamic_cast<HSAMemoryAPIInfo*>(m_pHsaApiInfo);

    if (nullptr != pHsaMemoryApiInfo)
    {
        if (nullptr != ppHsaMemoryApiInfoDataHandler)
        {
            *ppHsaMemoryApiInfoDataHandler = dynamic_cast<IHSAMemoryApiInfoDataHandler*>(this);
        }

        return true;
    }

    if (nullptr != ppHsaMemoryApiInfoDataHandler)
    {
        *ppHsaMemoryApiInfoDataHandler = nullptr;
    }

    return false;
}

bool HSAAPIInfoDataHandler::IsHSAMemoryTransferApi(IHSAMemoryTransferApiInfoDataHandler** ppHsaMemoryTransferApiInfoDataHandler)
{
    HSAMemoryTransferAPIInfo* pHsaMemoryTransferApiInfo = dynamic_cast<HSAMemoryTransferAPIInfo*>(m_pHsaApiInfo);

    if (nullptr != pHsaMemoryTransferApiInfo)
    {
        if (nullptr != ppHsaMemoryTransferApiInfoDataHandler)
        {
            *ppHsaMemoryTransferApiInfoDataHandler = dynamic_cast<IHSAMemoryTransferApiInfoDataHandler*>(this);
        }

        return true;
    }

    if (nullptr != ppHsaMemoryTransferApiInfoDataHandler)
    {
        *ppHsaMemoryTransferApiInfoDataHandler = nullptr;
    }

    return false;
}

bool HSAAPIInfoDataHandler::IsHSADispatchApi(IHSADispatchApiInfoDataHandler** ppHsaDispatchApiInfoDataHandler)
{
    HSADispatchInfo* pHsaDispatchApiInfo = dynamic_cast<HSADispatchInfo*>(m_pHsaApiInfo);

    if (nullptr != pHsaDispatchApiInfo)
    {
        if (nullptr != ppHsaDispatchApiInfoDataHandler)
        {
            *ppHsaDispatchApiInfoDataHandler = dynamic_cast<IHSADispatchApiInfoDataHandler*>(this);
        }

        return true;
    }

    if (nullptr != ppHsaDispatchApiInfoDataHandler)
    {
        *ppHsaDispatchApiInfoDataHandler = nullptr;
    }

    return false;
}

IAPIInfoDataHandler* HSAAPIInfoDataHandler::GetApiInfoDataHandler()
{
    return dynamic_cast<IAPIInfoDataHandler*>(this);
}

HSA_API_Type HSAAPIInfoDataHandler::GetHSAApiTypeId() const
{
    return (dynamic_cast<HSAAPIInfo*>(m_pHsaApiInfo))->m_apiID;
}

bool HSAAPIInfoDataHandler::IsApi() const
{
    return (dynamic_cast<HSAAPIInfo*>(m_pHsaApiInfo))->m_bIsAPI;
}

size_t HSAAPIInfoDataHandler::GetHSAMemoryApiSize() const
{
    size_t retVal = 0;
    HSAMemoryAPIInfo* pHsaMemoryInfo = dynamic_cast<HSAMemoryAPIInfo*>(m_pHsaApiInfo);

    if (nullptr != pHsaMemoryInfo)
    {
        retVal = pHsaMemoryInfo->m_size;
    }

    return retVal;
}

uint64_t HSAAPIInfoDataHandler::GetHSAMemoryTransferStartTime() const
{
    return (dynamic_cast<HSAMemoryTransferAPIInfo*>(m_pHsaApiInfo))->m_transferStartTime;
}

uint64_t HSAAPIInfoDataHandler::GetHSAMemoryTransferEndTime() const
{
    return (dynamic_cast<HSAMemoryTransferAPIInfo*>(m_pHsaApiInfo))->m_transferEndTime;
}

const char* HSAAPIInfoDataHandler::GetHSASrcAgentString() const
{
    return (dynamic_cast<HSAMemoryTransferAPIInfo*>(m_pHsaApiInfo))->m_strSrcAgent.c_str();
}

const char* HSAAPIInfoDataHandler::GetHSADestinationAgentString() const
{
    return (dynamic_cast<HSAMemoryTransferAPIInfo*>(m_pHsaApiInfo))->m_strDstAgent.c_str();
}

const char* HSAAPIInfoDataHandler::GetHSAKernelName() const
{
    return (dynamic_cast<HSADispatchInfo*>(m_pHsaApiInfo))->m_strKernelName.c_str();
}

unsigned long long HSAAPIInfoDataHandler::GetHSAKernelHandle() const
{
    return (dynamic_cast<HSADispatchInfo*>(m_pHsaApiInfo))->m_uiKernelHandle;
}

const char* HSAAPIInfoDataHandler::GetHSADeviceName() const
{
    return (dynamic_cast<HSADispatchInfo*>(m_pHsaApiInfo))->m_strDeviceName.c_str();
}

const char* HSAAPIInfoDataHandler::GetHSADeviceHandleString() const
{
    return (dynamic_cast<HSADispatchInfo*>(m_pHsaApiInfo))->m_strDeviceHandle.c_str();
}

unsigned int HSAAPIInfoDataHandler::GetHSAQueueIndex() const
{
    return (dynamic_cast<HSADispatchInfo*>(m_pHsaApiInfo))->m_queueIndex;
}

const char* HSAAPIInfoDataHandler::GetHSAQueueHandleString() const
{
    return (dynamic_cast<HSADispatchInfo*>(m_pHsaApiInfo))->m_strQueueHandle.c_str();
}

const char* HSAAPIInfoDataHandler::GetHSAWorkGroupSizeString() const
{
    return (dynamic_cast<HSADispatchInfo*>(m_pHsaApiInfo))->m_strGroupWorkSize.c_str();
}

const char* HSAAPIInfoDataHandler::GetHSAGlobalWorkGroupSize() const
{
    return (dynamic_cast<HSADispatchInfo*>(m_pHsaApiInfo))->m_strGlobalWorkSize.c_str();
}

void HSAAPIInfoDataHandler::GetHSALocalGroups(size_t** ppHsaLocalGroups, size_t& groupCount) const
{
    groupCount = HSA_MAX_DIM;
    *ppHsaLocalGroups = new(std::nothrow) size_t[groupCount];

    if (nullptr != (*ppHsaLocalGroups))
    {
        m_pGroupList.push_back(*ppHsaLocalGroups);
        memcpy(*ppHsaLocalGroups, (dynamic_cast<HSADispatchInfo*>(m_pHsaApiInfo))->m_localGroup, sizeof(size_t) * groupCount);
    }
}

void HSAAPIInfoDataHandler::GetHSAGlobalGroups(size_t** ppHsaGlobalGroups, size_t& groupCount) const
{
    groupCount = HSA_MAX_DIM;
    *ppHsaGlobalGroups = new(std::nothrow) size_t[groupCount];

    if (nullptr != (*ppHsaGlobalGroups))
    {
        m_pGroupList.push_back(*ppHsaGlobalGroups);
        memcpy(*ppHsaGlobalGroups, (dynamic_cast<HSADispatchInfo*>(m_pHsaApiInfo))->m_globalGroup, sizeof(size_t) * groupCount);
    }
}

size_t HSAAPIInfoDataHandler::GetHSAGroupDimension() const
{
    return (dynamic_cast<HSADispatchInfo*>(m_pHsaApiInfo))->m_uDim;
}

HSAAPIInfoDataHandler::~HSAAPIInfoDataHandler()
{
    for (std::vector<size_t*>::iterator it = m_pGroupList.begin(); it != m_pGroupList.end(); ++it)
    {
        delete(*it);
    }
}
