//==============================================================================
// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief CLApiInfoDataHandler Implementation
//==============================================================================

#include "CLApiInfoDataHandlerImp.h"

ULONGLONG CLAPIInfoDataHandler::GetApiStartTime() const
{
    return m_pCLApiInfo->m_ullStart;
}

ULONGLONG CLAPIInfoDataHandler::GetApiEndTime() const
{
    return m_pCLApiInfo->m_ullEnd;
}

unsigned CLAPIInfoDataHandler::GetApiSequenceId() const
{
    return m_pCLApiInfo->m_uiSeqID;
}

unsigned CLAPIInfoDataHandler::GetApiDisplaySequenceId() const
{
    return m_pCLApiInfo->m_uiDisplaySeqID;
}

bool CLAPIInfoDataHandler::IsApiSequenceIdDisplayble() const
{
    return m_pCLApiInfo->m_bHasDisplayableSeqId;
}

osThreadId CLAPIInfoDataHandler::GetApiThreadId() const
{
    return m_pCLApiInfo->m_tid;
}

const char* CLAPIInfoDataHandler::GetApiRetString() const
{
    return m_pCLApiInfo->m_strRet.c_str();
}

const char* CLAPIInfoDataHandler::GetApiNameString() const
{
    return m_pCLApiInfo->m_strName.c_str();
}

const char* CLAPIInfoDataHandler::GetApiArgListString() const
{
    return m_pCLApiInfo->m_argList.c_str();
}

bool CLAPIInfoDataHandler::IsCLMemoryApiInfo(ICLMemApiInfoDataHandler** ppClMemApiInfoDataHandler)
{
    CLMemAPIInfo* pClMemoryApiInfo = dynamic_cast<CLMemAPIInfo*>(m_pCLApiInfo);

    if (nullptr != pClMemoryApiInfo)
    {
        if (nullptr != ppClMemApiInfoDataHandler)
        {
            *ppClMemApiInfoDataHandler = dynamic_cast<ICLMemApiInfoDataHandler*>(this);
        }

        return true;
    }

    if (nullptr != ppClMemApiInfoDataHandler)
    {
        *ppClMemApiInfoDataHandler = nullptr;
    }

    return false;
}

bool CLAPIInfoDataHandler::IsCLEnqueueOtherOperations(ICLOtherEnqueueApiInfoDataHandler** ppClEnqueueOtherOperations)
{
    CLOtherEnqueueAPIInfo* pClOtherEnQueueApiInfo = dynamic_cast<CLOtherEnqueueAPIInfo*>(m_pCLApiInfo);

    if (nullptr != pClOtherEnQueueApiInfo)
    {
        if (nullptr != ppClEnqueueOtherOperations)
        {
            *ppClEnqueueOtherOperations = dynamic_cast<ICLOtherEnqueueApiInfoDataHandler*>(this);
        }

        return true;
    }

    if (nullptr != ppClEnqueueOtherOperations)
    {
        *ppClEnqueueOtherOperations = nullptr;
    }

    return false;
}

bool CLAPIInfoDataHandler::IsCLEnqueueAPI(ICLEnqueueApiInfoDataHandler** ppClEnqueueApiInfoDataHandler)
{
    CLEnqueueAPI* pClEnQueueApiInfo = dynamic_cast<CLEnqueueAPI*>(m_pCLApiInfo);

    if (nullptr != pClEnQueueApiInfo)
    {
        if (nullptr != ppClEnqueueApiInfoDataHandler)
        {
            *ppClEnqueueApiInfoDataHandler = dynamic_cast<ICLEnqueueApiInfoDataHandler*>(this);
        }

        return true;
    }

    if (nullptr != ppClEnqueueApiInfoDataHandler)
    {
        *ppClEnqueueApiInfoDataHandler = nullptr;
    }

    return false;
}

bool CLAPIInfoDataHandler::IsCLDataEnqueueApi(ICLDataEnqueueApiInfoDataHandler** ppClDataEnqueueApiInfoDataHandler)
{
    CLDataEnqueueAPIInfo* pClDataEnQueueApiInfo = dynamic_cast<CLDataEnqueueAPIInfo*>(m_pCLApiInfo);

    if (nullptr != pClDataEnQueueApiInfo)
    {
        if (nullptr != ppClDataEnqueueApiInfoDataHandler)
        {
            *ppClDataEnqueueApiInfoDataHandler = dynamic_cast<ICLDataEnqueueApiInfoDataHandler*>(this);
        }

        return true;
    }

    if (nullptr != ppClDataEnqueueApiInfoDataHandler)
    {
        *ppClDataEnqueueApiInfoDataHandler = nullptr;
    }

    return false;
}

bool CLAPIInfoDataHandler::IsCLKernelApiInfo(ICLKernelApiInfoDataHandler** ppClKernelApiInfoDataHandler)
{
    CLKernelAPIInfo* pClKernelApiInfo = dynamic_cast<CLKernelAPIInfo*>(m_pCLApiInfo);

    if (nullptr != pClKernelApiInfo)
    {
        if (nullptr != ppClKernelApiInfoDataHandler)
        {
            *ppClKernelApiInfoDataHandler = dynamic_cast<ICLKernelApiInfoDataHandler*>(this);
        }

        return true;
    }

    if (nullptr != ppClKernelApiInfoDataHandler)
    {
        *ppClKernelApiInfoDataHandler = nullptr;
    }

    return false;
}

IAPIInfoDataHandler* CLAPIInfoDataHandler::GetApiInfoDataHandler()
{
    return dynamic_cast<IAPIInfoDataHandler*>(this);
}

CLAPIType CLAPIInfoDataHandler::GetCLApiType() const
{
    return (dynamic_cast<CLAPIInfo*>(m_pCLApiInfo))->m_Type;
}

unsigned int CLAPIInfoDataHandler::GetCLApiId() const
{
    return (dynamic_cast<CLAPIInfo*>(m_pCLApiInfo))->m_uiAPIID;
}

const char* CLAPIInfoDataHandler::GetCLApiComment() const
{
    return (dynamic_cast<CLAPIInfo*>(m_pCLApiInfo))->m_strComment.c_str();
}

bool CLAPIInfoDataHandler::IsCLEnqueueInformationMissing() const
{
    return (dynamic_cast<CLEnqueueAPI*>(m_pCLApiInfo))->m_bInfoMissing;
}

const char* CLAPIInfoDataHandler::GetCLCommandTypeString() const
{
    return (dynamic_cast<CLEnqueueAPI*>(m_pCLApiInfo))->m_strCMDType.c_str();
}

const char* CLAPIInfoDataHandler::GetCLDeviceNameString() const
{
    return (dynamic_cast<CLEnqueueAPI*>(m_pCLApiInfo))->m_strDevice.c_str();
}

const char* CLAPIInfoDataHandler::GetCLContextHandleString() const
{
    return (dynamic_cast<CLEnqueueAPI*>(m_pCLApiInfo))->m_strCntxHandle.c_str();
}

const char* CLAPIInfoDataHandler::GetCLCommandQueueHandleString() const
{
    return (dynamic_cast<CLEnqueueAPI*>(m_pCLApiInfo))->m_strCmdQHandle.c_str();
}

const char* CLAPIInfoDataHandler::GetCLEventHandleString() const
{
    return (dynamic_cast<CLEnqueueAPI*>(m_pCLApiInfo))->m_strEventHandle.c_str();
}

ULONGLONG CLAPIInfoDataHandler::GetCLQueueTimestamp() const
{
    return (dynamic_cast<CLEnqueueAPI*>(m_pCLApiInfo))->m_ullQueue;
}

ULONGLONG CLAPIInfoDataHandler::GetCLSubmitTimestamp() const
{
    return (dynamic_cast<CLEnqueueAPI*>(m_pCLApiInfo))->m_ullSubmit;
}

ULONGLONG CLAPIInfoDataHandler::GetCLRunningTimestamp() const
{
    return (dynamic_cast<CLEnqueueAPI*>(m_pCLApiInfo))->m_ullRunning;
}

ULONGLONG CLAPIInfoDataHandler::GetCLCompleteTimestamp() const
{
    return (dynamic_cast<CLEnqueueAPI*>(m_pCLApiInfo))->m_ullComplete;
}

unsigned int CLAPIInfoDataHandler::GetCLCommandTypeEnum() const
{
    return (dynamic_cast<CLEnqueueAPI*>(m_pCLApiInfo))->m_uiCMDType;
}

unsigned int CLAPIInfoDataHandler::GetCLQueueId() const
{
    return (dynamic_cast<CLEnqueueAPI*>(m_pCLApiInfo))->m_uiQueueID;
}

unsigned int CLAPIInfoDataHandler::GetCLContextId() const
{
    return (dynamic_cast<CLEnqueueAPI*>(m_pCLApiInfo))->m_uiContextID;
}

unsigned int CLAPIInfoDataHandler::GetCLMemoryTransferSize() const
{
    return (dynamic_cast<CLMemAPIInfo*>(m_pCLApiInfo))->m_uiTransferSize;
}

unsigned int CLAPIInfoDataHandler::GetCLDataTransferSize() const
{
    return (dynamic_cast<CLDataEnqueueAPIInfo*>(m_pCLApiInfo))->m_uiDataSize;
}

const char* CLAPIInfoDataHandler::GetCLKernelHandleString() const
{
    return (dynamic_cast<CLKernelAPIInfo*>(m_pCLApiInfo))->m_strKernelHandle.c_str();
}

const char* CLAPIInfoDataHandler::GetCLKernelNameString() const
{
    return (dynamic_cast<CLKernelAPIInfo*>(m_pCLApiInfo))->m_strKernelName.c_str();
}

const char* CLAPIInfoDataHandler::GetCLKernelWorkGroupSize() const
{
    return (dynamic_cast<CLKernelAPIInfo*>(m_pCLApiInfo))->m_strGroupWorkSize.c_str();
}

const char* CLAPIInfoDataHandler::GetCLKernelGlobalWorkGroupSize() const
{
    return (dynamic_cast<CLKernelAPIInfo*>(m_pCLApiInfo))->m_strGlobalWorkSize.c_str();
}

