//==============================================================================
// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Occupancy Info Data Handler Implmentation
//==============================================================================

#include "OccupancyInfoDataHandlerImp.h"
#include "../sprofile/OccupancyChart.h"
#include <cstring>

unsigned int OccupancyInfoDataHandler::GetThreadId() const
{
    return m_occupancyInfo.m_nTID;
}

const char* OccupancyInfoDataHandler::GetKernelName() const
{
    return m_occupancyInfo.m_strKernelName.c_str();
}

const char* OccupancyInfoDataHandler::GetDeviceName() const
{
    return m_occupancyInfo.m_strDeviceName.c_str();
}

unsigned int OccupancyInfoDataHandler::GetDeviceGfxIp() const
{
    return m_occupancyInfo.m_nDeviceGfxIpVer;
}

unsigned int OccupancyInfoDataHandler::GetComputeUnitCount() const
{
    return m_occupancyInfo.m_nNbrComputeUnits;
}

unsigned int OccupancyInfoDataHandler::GetMaxWavePerCU() const
{
    return m_occupancyInfo.m_nMaxWavesPerCU;
}

unsigned int OccupancyInfoDataHandler::GetMaxWorkGroupPerCU() const
{
    return m_occupancyInfo.m_nMaxWGPerCU;
}

unsigned int OccupancyInfoDataHandler::GetMaxVGPRs() const
{
    return m_occupancyInfo.m_nMaxVGPRS;
}

unsigned int OccupancyInfoDataHandler::GetMaxSGPRs() const
{
    return m_occupancyInfo.m_nMaxSGPRS;
}

unsigned int OccupancyInfoDataHandler::GetMaxLDS() const
{
    return m_occupancyInfo.m_nMaxLDS;
}

unsigned int OccupancyInfoDataHandler::GetUsedVGPRs() const
{
    return m_occupancyInfo.m_nUsedVGPRS;
}

unsigned int OccupancyInfoDataHandler::GetUsedSGPRs() const
{
    return m_occupancyInfo.m_nUsedSGPRS;
}

unsigned int OccupancyInfoDataHandler::GetUsedLDS() const
{
    return m_occupancyInfo.m_nUsedLDS;
}

unsigned int OccupancyInfoDataHandler::GetWavefrontSize() const
{
    return m_occupancyInfo.m_nWavefrontSize;
}

unsigned int OccupancyInfoDataHandler::GetWorkGroupSize() const
{
    return m_occupancyInfo.m_nWorkgroupSize;
}

unsigned int OccupancyInfoDataHandler::GetWavesPerWorkGroup() const
{
    return m_occupancyInfo.m_nWavesPerWG;
}

unsigned int OccupancyInfoDataHandler::GetMaxWorkGroupSize() const
{
    return m_occupancyInfo.m_nMaxWGSize;
}

unsigned int OccupancyInfoDataHandler::GetMaxWavesPerWorkGroup() const
{
    return m_occupancyInfo.m_nMaxWavesPerWG;
}

unsigned int OccupancyInfoDataHandler::GetGlobalWorkSize() const
{
    return m_occupancyInfo.m_nGlobalWorkSize;
}

unsigned int OccupancyInfoDataHandler::GetMaxGloablWorkSize() const
{
    return m_occupancyInfo.m_nMaxGlobalWorkSize;
}

unsigned int OccupancyInfoDataHandler::GetWavesLimitByVGPR() const
{
    return m_occupancyInfo.m_nVGPRLimitedWaveCount;
}

unsigned int OccupancyInfoDataHandler::GetWavesLimitBySGPR() const
{
    return m_occupancyInfo.m_nSGPRLimitedWaveCount;
}

unsigned int OccupancyInfoDataHandler::GetWavesLimitByLDS() const
{
    return m_occupancyInfo.m_nLDSLimitedWaveCount;
}

unsigned int OccupancyInfoDataHandler::GetWavesLimitByWorkGroup() const
{
    return m_occupancyInfo.m_nWGLimitedWaveCount;
}

float OccupancyInfoDataHandler::GetOccupancy() const
{
    return m_occupancyInfo.m_fOccupancy;
}

GDT_HW_GENERATION OccupancyInfoDataHandler::GetGeneration() const
{
    return m_occupancyInfo.m_gen;
}

unsigned int OccupancyInfoDataHandler::GetSimdsPerCU() const
{
    return m_occupancyInfo.m_nSimdsPerCU;
}

bool OccupancyInfoDataHandler::GenerateOccupancyChart(const char* pOutputFile, char** ppErrorMessage) const
{
    std::string tmpErrorMessage;
    bool success = false;

    if (nullptr != pOutputFile)
    {
        success = ::GenerateOccupancyChart(m_occupancyInfo, pOutputFile, tmpErrorMessage);
    }

    if (success != true)
    {
        if (nullptr != m_pErrorMessageResource)
        {
            delete[] m_pErrorMessageResource;
        }

        if (!tmpErrorMessage.empty())
        {
            *ppErrorMessage = new(std::nothrow) char[tmpErrorMessage.size() + 1];

            if (nullptr != *ppErrorMessage)
            {
                memcpy(*ppErrorMessage, tmpErrorMessage.c_str(), tmpErrorMessage.size());
                (*ppErrorMessage)[tmpErrorMessage.size()] = '\0';
                m_pErrorMessageResource = *ppErrorMessage;
            }
        }
        else
        {
            *ppErrorMessage = new(std::nothrow) char[1];

            if (nullptr != ppErrorMessage)
            {
                *ppErrorMessage[0] = '\0';
                m_pErrorMessageResource = *ppErrorMessage;
            }
        }
    }

    return success;
}

OccupancyInfoDataHandler::~OccupancyInfoDataHandler()
{
    delete[] m_pErrorMessageResource;
}
