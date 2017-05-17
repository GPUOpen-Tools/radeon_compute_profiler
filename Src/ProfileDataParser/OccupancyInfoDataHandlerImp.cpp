//==============================================================================
// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Occupancy Info Data Handler Implmentation
//==============================================================================

#include "OccupancyInfoDataHandlerImp.h"
#include "../sprofile/OccupancyChart.h"

unsigned int OccupancyInfoDataHandler::GetThreadId() const
{
    return m_occupancyInfo.m_nTID;
}

std::string OccupancyInfoDataHandler::GetKernelName() const
{
    return m_occupancyInfo.m_strKernelName;
}

std::string OccupancyInfoDataHandler::GetDeviceName() const
{
    return m_occupancyInfo.m_strDeviceName;
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

bool OccupancyInfoDataHandler::GenerateOccupancyChart(const std::string outputFile, std::string& errorMessage) const
{
    return ::GenerateOccupancyChart(m_occupancyInfo, outputFile, errorMessage);
}
