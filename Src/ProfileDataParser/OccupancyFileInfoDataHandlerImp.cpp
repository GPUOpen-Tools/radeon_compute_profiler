//==============================================================================
// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Occupancy File Info Data Data Handler Interface Implementation
//==============================================================================

// Common
#include <DeviceInfoUtils.h>

// profiler common
#include <ProfilerOutputFileDefs.h>
#include <StringUtils.h>

#include "OccupancyFileInfoDataHandlerImp.h"
#include "OccupancyInfoDataHandlerImp.h"

OccupancyFileInfoDataHandler::OccupancyFileInfoDataHandler(std::string& occupancyFileName):
    m_occupancyFileName(occupancyFileName),
    m_bIsDataReady(false),
    m_occupancyFileMajorVersion(0u),
    m_occupancyFileMinorVersion(0u)
{
}

bool OccupancyFileInfoDataHandler::ParseOccupancyFile(const std::string& occupancyFile)
{
    bool success = m_bIsDataReady;

    if (!m_bIsDataReady)
    {
        CSVFileParser parser;
        parser.AddListener(this);
        parser.LoadFile(occupancyFile.c_str());

        if (parser.Parse())
        {
            success = true;
            m_headerList = parser.GetColumns();
            GenerateKernelInfoByThreadId();

            std::vector<std::string> comments = parser.GetHeaders();
            bool foundVersion = false;

            for (std::vector<std::string>::const_iterator it = comments.begin(); it != comments.end() && !foundVersion; ++it)
            {
                if (0 == it->compare(FILE_HEADER_PROFILE_FILE_VERSION))
                {
                    foundVersion = true;

                    if (!StringUtils::ParseMajorMinorVersion(*it, m_occupancyFileMajorVersion, m_occupancyFileMinorVersion))
                    {
                        m_occupancyFileMajorVersion = m_occupancyFileMinorVersion = 0u;
                    }
                }
            }
        }

        m_bIsDataReady = success;
    }

    return success;
}

void OccupancyFileInfoDataHandler::OnParse(CSVRow* pCsvRow, bool& stopParsing)
{
    SP_UNREFERENCED_PARAMETER(stopParsing);

    auto ColumnStrOutToUint = [&](std::string columnName)->unsigned int
    {
        std::string tempString = pCsvRow->GetRowData(columnName);
        unsigned int outVal;

        if (!StringUtils::Parse(tempString, outVal))
        {
            outVal = 0u;
        }

        return outVal;
    };

    if (nullptr != pCsvRow)
    {
        OccupancyInfoDataHandler* pTmpOccupancyInfo = new OccupancyInfoDataHandler();
        pTmpOccupancyInfo->m_occupancyInfo.m_nTID = ColumnStrOutToUint(OCCUPANCY_COLUMN_NAME_THREADID);
        pTmpOccupancyInfo->m_occupancyInfo.m_strKernelName = pCsvRow->GetRowData(OCCUPANCY_COLUMN_NAME_KERNELNAME);
        pTmpOccupancyInfo->m_occupancyInfo.m_strDeviceName = pCsvRow->GetRowData(OCCUPANCY_COLUMN_NAME_DEVICENAME);
        pTmpOccupancyInfo->m_occupancyInfo.m_nDeviceGfxIpVer = ColumnStrOutToUint(OCCUPANCY_COLUMN_NAME_GRAPHICSIPVERSION);
        pTmpOccupancyInfo->m_occupancyInfo.m_nNbrComputeUnits = ColumnStrOutToUint(OCCUPANCY_COLUMN_NAME_NUMBEROFCOMPUTEUNITS);
        pTmpOccupancyInfo->m_occupancyInfo.m_nMaxWavesPerCU = ColumnStrOutToUint(OCCUPANCY_COLUMN_NAME_MAXNUMBEROFWAVEFRONTSPERCU);
        pTmpOccupancyInfo->m_occupancyInfo.m_nMaxWGPerCU = ColumnStrOutToUint(OCCUPANCY_COLUMN_NAME_MAXNUMBEROFWORKGROUPPERCU);
        pTmpOccupancyInfo->m_occupancyInfo.m_nMaxVGPRS = ColumnStrOutToUint(OCCUPANCY_COLUMN_NAME_MAXNUMBEROFVGPR);
        pTmpOccupancyInfo->m_occupancyInfo.m_nMaxSGPRS = ColumnStrOutToUint(OCCUPANCY_COLUMN_NAME_MAXNUMBEROFSGPR);
        pTmpOccupancyInfo->m_occupancyInfo.m_nMaxLDS = ColumnStrOutToUint(OCCUPANCY_COLUMN_NAME_MAXAMOUNTOFLDS);
        pTmpOccupancyInfo->m_occupancyInfo.m_nUsedVGPRS = ColumnStrOutToUint(OCCUPANCY_COLUMN_NAME_NUMBEROFVGPRUSED);
        pTmpOccupancyInfo->m_occupancyInfo.m_nUsedSGPRS = ColumnStrOutToUint(OCCUPANCY_COLUMN_NAME_NUMBEROFSGPRUSED);
        pTmpOccupancyInfo->m_occupancyInfo.m_nUsedLDS = ColumnStrOutToUint(OCCUPANCY_COLUMN_NAME_AMOUNTOFLDSUSED);
        pTmpOccupancyInfo->m_occupancyInfo.m_nWavefrontSize = ColumnStrOutToUint(OCCUPANCY_COLUMN_NAME_SIZEOFWAVEFRONT);
        pTmpOccupancyInfo->m_occupancyInfo.m_nWorkgroupSize = ColumnStrOutToUint(OCCUPANCY_COLUMN_NAME_WORKGROUPSIZE);
        pTmpOccupancyInfo->m_occupancyInfo.m_nWavesPerWG = ColumnStrOutToUint(OCCUPANCY_COLUMN_NAME_WAVEFRONTSPERWORKGROUP);
        pTmpOccupancyInfo->m_occupancyInfo.m_nMaxWGSize = ColumnStrOutToUint(OCCUPANCY_COLUMN_NAME_MAXWORKGROUPSIZE);
        pTmpOccupancyInfo->m_occupancyInfo.m_nMaxWavesPerWG = ColumnStrOutToUint(OCCUPANCY_COLUMN_NAME_MAXWAVEFRONTSPERWORKGROUP);
        pTmpOccupancyInfo->m_occupancyInfo.m_nGlobalWorkSize = ColumnStrOutToUint(OCCUPANCY_COLUMN_NAME_GLOBALWORKSIZE);
        pTmpOccupancyInfo->m_occupancyInfo.m_nMaxGlobalWorkSize = ColumnStrOutToUint(OCCUPANCY_COLUMN_NAME_MAXIMUMGLOBALWORKSIZE);
        pTmpOccupancyInfo->m_occupancyInfo.m_nVGPRLimitedWaveCount = ColumnStrOutToUint(OCCUPANCY_COLUMN_NAME_NBRVGPRLIMITEDWAVES);
        pTmpOccupancyInfo->m_occupancyInfo.m_nSGPRLimitedWaveCount = ColumnStrOutToUint(OCCUPANCY_COLUMN_NAME_NBRSGPRLIMITEDWAVES);
        pTmpOccupancyInfo->m_occupancyInfo.m_nLDSLimitedWaveCount = ColumnStrOutToUint(OCCUPANCY_COLUMN_NAME_NBRLDSLIMITEDWAVES);
        pTmpOccupancyInfo->m_occupancyInfo.m_nWGLimitedWaveCount = ColumnStrOutToUint(OCCUPANCY_COLUMN_NAME_NBROFWGLIMITEDWAVES);
        pTmpOccupancyInfo->m_occupancyInfo.m_nSimdsPerCU = ColumnStrOutToUint(OCCUPANCY_COLUMN_NAME_SIMDSPERCU);

        if (0 == pTmpOccupancyInfo->m_occupancyInfo.m_nSimdsPerCU) // hardcode to 4 if the occupancy file does not have this data
        {
            pTmpOccupancyInfo->m_occupancyInfo.m_nSimdsPerCU = 4;
        }

        pTmpOccupancyInfo->m_occupancyInfo.m_fOccupancy = static_cast<float>(ColumnStrOutToUint(OCCUPANCY_COLUMN_NAME_KERNELOCCUPANCY));

        if(pTmpOccupancyInfo->m_occupancyInfo.m_nDeviceGfxIpVer == 0)
        {
            AMDTDeviceInfoUtils::Instance()->GetHardwareGeneration(pTmpOccupancyInfo->m_occupancyInfo.m_strDeviceName.c_str(), pTmpOccupancyInfo->m_occupancyInfo.m_gen);
        }
        else
        {
            AMDTDeviceInfoUtils::Instance()->GfxIPVerToHwGeneration(pTmpOccupancyInfo->m_occupancyInfo.m_nDeviceGfxIpVer, pTmpOccupancyInfo->m_occupancyInfo.m_gen);
        }

        m_occupancyInfoList.push_back(pTmpOccupancyInfo);
    }
}

bool OccupancyFileInfoDataHandler::IsDataReady() const
{
    return m_bIsDataReady;
}

void OccupancyFileInfoDataHandler::GetOccupancyFileVersion(unsigned int& major, unsigned int& minor) const
{
    major = m_occupancyFileMajorVersion;
    minor = m_occupancyFileMinorVersion;
}

std::vector<ColumnName> OccupancyFileInfoDataHandler::GetHeaderInOrder() const
{
    return m_headerList;
}

std::map<osThreadId, KernelCount> OccupancyFileInfoDataHandler::GetKernelCountByThreadId() const
{
    std::map<osThreadId, KernelCount> kernelCountByThreadIdMap;

    if (m_bIsDataReady)
    {
        OccupancyInfoByThreadId::const_iterator const_iter;

        for (const_iter = m_kernelCountInfoByThreadId.begin(); const_iter != m_kernelCountInfoByThreadId.end(); ++const_iter)
        {
            kernelCountByThreadIdMap.insert(std::pair<osThreadId, KernelCount>(const_iter->first, static_cast<unsigned int>(const_iter->second.size())));
        }
    }

    return kernelCountByThreadIdMap;
}

const IOccupancyInfoDataHandler* OccupancyFileInfoDataHandler::GetOccupancyInfoDataHandler(osThreadId threadId, unsigned int index) const
{
    IOccupancyInfoDataHandler* pRetHandler = nullptr;
    OccupancyInfoByThreadId::const_iterator occupancyInfoIter = m_kernelCountInfoByThreadId.find(threadId);

    if (occupancyInfoIter != m_kernelCountInfoByThreadId.end() && occupancyInfoIter->second.size() < index)
    {
        pRetHandler = occupancyInfoIter->second[index];
    }

    return pRetHandler;
}

std::vector<const IOccupancyInfoDataHandler*> OccupancyFileInfoDataHandler::GetOccupancyInfoByThreadId(osThreadId threadId) const
{
    std::vector<const IOccupancyInfoDataHandler*> retList;
    OccupancyInfoByThreadId::const_iterator occupancyInfoIter = m_kernelCountInfoByThreadId.find(threadId);

    if (occupancyInfoIter != m_kernelCountInfoByThreadId.end())
    {
        for (std::vector<IOccupancyInfoDataHandler*>::const_iterator it = occupancyInfoIter->second.begin(); it != occupancyInfoIter->second.end(); ++it)
        {
            retList.push_back(*it);
        }
    }

    return retList;
}

void OccupancyFileInfoDataHandler::ReleaseData()
{
    m_bIsDataReady = false;
    OccupancyInfoByThreadId::iterator iter;

    for (iter = m_kernelCountInfoByThreadId.begin(); iter != m_kernelCountInfoByThreadId.end(); ++iter)
    {
        for (std::vector<IOccupancyInfoDataHandler*>::iterator infoListIter = iter->second.begin(); infoListIter != iter->second.end() && nullptr != (*infoListIter); ++infoListIter)
        {
            delete(*infoListIter);
        }

        iter->second.clear();
    }

    m_kernelCountInfoByThreadId.clear();
    m_occupancyInfoList.clear();
}

OccupancyFileInfoDataHandler::~OccupancyFileInfoDataHandler()
{
    if (m_bIsDataReady)
    {
        OccupancyInfoByThreadId::iterator iter;

        for (iter = m_kernelCountInfoByThreadId.begin(); iter != m_kernelCountInfoByThreadId.end(); ++iter)
        {
            for (std::vector<IOccupancyInfoDataHandler*>::iterator infoListIter = iter->second.begin(); infoListIter != iter->second.end() && nullptr != (*infoListIter); ++infoListIter)
            {
                delete(*infoListIter);
            }

            iter->second.clear();
        }

        m_kernelCountInfoByThreadId.clear();
        m_occupancyInfoList.clear();
    }
}

void OccupancyFileInfoDataHandler::GenerateKernelInfoByThreadId()
{
    for (std::vector<IOccupancyInfoDataHandler*>::iterator it = m_occupancyInfoList.begin(); it != m_occupancyInfoList.end(); ++it)
    {
        OccupancyInfoByThreadId::iterator occupancyInfoByThreadIter;
        occupancyInfoByThreadIter = m_kernelCountInfoByThreadId.find((*it)->GetThreadId());

        if (occupancyInfoByThreadIter != m_kernelCountInfoByThreadId.end())
        {
            occupancyInfoByThreadIter->second.push_back(*it);
        }
        else
        {
            std::vector<IOccupancyInfoDataHandler*> newList;
            newList.push_back(*it);
            m_kernelCountInfoByThreadId.insert(std::pair<osThreadId, std::vector<IOccupancyInfoDataHandler*>>((*it)->GetThreadId(), newList));
        }
    }
}

