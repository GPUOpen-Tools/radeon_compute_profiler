//==============================================================================
// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Occupancy File Info Data Data Handler Interface Implementation
//==============================================================================

// std
#include <fstream>
#include <functional>

// Common
#include <DeviceInfoUtils.h>

// profiler common
#include <ProfilerOutputFileDefs.h>
#include <StringUtils.h>

#include "OccupancyFileInfoDataHandlerImp.h"
#include "OccupancyInfoDataHandlerImp.h"
#include "AtpDataHandlerImp.h"

OccupancyFileInfoDataHandler::OccupancyFileInfoDataHandler(std::string& occupancyFileName):
    m_occupancyFileName(occupancyFileName),
    m_bIsDataReady(false),
    m_ppHeaderList(nullptr),
    m_headerColumnCount(0u),
    m_occupancyFileMajorVersion(0u),
    m_occupancyFileMinorVersion(0u)
{
}

bool OccupancyFileInfoDataHandler::ParseOccupancyFile(const char* pOccupancyFile)
{
    bool success = m_bIsDataReady;

    if (!m_bIsDataReady)
    {
        success = Parse(pOccupancyFile);

        if (success)
        {
            GenerateKernelInfoByThreadId();
        }

        m_bIsDataReady = success;
    }

    return success;
}

bool OccupancyFileInfoDataHandler::Parse(const char* pOccupancyFile)
{
    bool success = false;
    bool foundVersion = false;
    std::ifstream occupancyFileStream;
    occupancyFileStream.open(pOccupancyFile, std::ifstream::in);
    typedef size_t StringHash;
    std::map<StringHash, unsigned int> stringHashMap;
    bool bFoundHeader = false;
    size_t kernelCount = 0u;
    size_t currentKernelCount = 0u;
    std::string listSeparator;

    auto GetColumnIndex = [&](std::string columnName)->size_t
    {
        std::hash<std::string> stringHash;
        size_t index = std::string::npos;
        StringHash columnNameHash = stringHash(columnName);

        if (stringHashMap.find(columnNameHash) != stringHashMap.end())
        {
            index = stringHashMap[columnNameHash];
        }

        return index;
    };

    std::vector<std::string> columnData;
    auto ColumnStrOutToUint = [&](std::string columnName)->unsigned int
    {

        unsigned int outVal;
        std::string tempString = columnData[GetColumnIndex(columnName)];

        if (!StringUtils::Parse(tempString, outVal))
        {
            outVal = 0u;
        }

        return outVal;
    };

    if (occupancyFileStream.is_open())
    {
        while (!occupancyFileStream.eof())
        {
            constexpr size_t lineSizeBuffer = 2048;
            char lineBuffer[lineSizeBuffer];
            occupancyFileStream.getline(lineBuffer, lineSizeBuffer);

            if (occupancyFileStream.eof())
            {
                break;
            }

            if ('#' == lineBuffer[0])
            {
                // Comments Section of the Occupancy File
                std::string occupancyHeader(lineBuffer);

                std::string profilerVersionNameString(FILE_HEADER_PROFILER_VERSION);
                size_t profilerVersionNameStringPosition = occupancyHeader.find(profilerVersionNameString);

                if (std::string::npos != profilerVersionNameStringPosition)
                {
                    std::string profilerVersionString = std::string(occupancyHeader.begin() + profilerVersionNameStringPosition + profilerVersionNameString.size() + 1, occupancyHeader.end());

                    if (!StringUtils::ParseMajorMinorVersion(profilerVersionString, m_occupancyFileMajorVersion, m_occupancyFileMinorVersion))
                    {
                        m_occupancyFileMajorVersion = m_occupancyFileMinorVersion = 0u;
                    }
                    else
                    {
                        foundVersion = true;
                    }
                }

                std::string kernelCountNameString(FILE_HEADER_KERNEL_COUNT);
                size_t kernelCountStringPosition = occupancyHeader.find(kernelCountNameString);

                if (std::string::npos != kernelCountStringPosition)
                {
                    std::string kernelCountString = std::string(occupancyHeader.begin() + kernelCountStringPosition + kernelCountNameString.size() + 1, occupancyHeader.end());

                    if (!StringUtils::Parse(kernelCountString, kernelCount))
                    {
                        kernelCount = 0u;
                    }
                }

                std::string listSeparatorNameString(FILE_HEADER_LIST_SEPARATOR);
                size_t listSeparatorPosition = occupancyHeader.find(listSeparatorNameString);

                if (std::string::npos != listSeparatorPosition)
                {
                    listSeparator = std::string(occupancyHeader.begin() + listSeparatorPosition + listSeparatorNameString.size() + 1, occupancyHeader.end());
                }
            }
            else
            {
                if (listSeparator.empty())
                {
                    listSeparator.push_back(OCCUPANCY_DEFAULT_LIST_SEPARATOR_CHAR);
                }

                if (!bFoundHeader)
                {
                    bFoundHeader = true;
                    std::vector<std::string> columnHeaders;
                    StringUtils::Split(columnHeaders, lineBuffer, listSeparator);
                    m_headerColumnCount = static_cast<unsigned int>(columnHeaders.size());
                    m_ppHeaderList = new(std::nothrow) char* [m_headerColumnCount];

                    if (nullptr != m_ppHeaderList)
                    {
                        unsigned int headerIndex = 0;
                        std::hash<std::string> stringHash;

                        for (std::vector<std::string>::iterator columnHeaderIter = columnHeaders.begin(); columnHeaderIter != columnHeaders.end(); ++columnHeaderIter)
                        {
                            StringHash columnNameHash = stringHash(*columnHeaderIter);
                            stringHashMap.insert(std::pair<StringHash, unsigned int>(columnNameHash, headerIndex));
                            m_ppHeaderList[headerIndex] = new(std::nothrow) char[columnHeaderIter->size() + 1];

                            if (nullptr != m_ppHeaderList)
                            {
                                memcpy(m_ppHeaderList[headerIndex], columnHeaderIter->c_str(), columnHeaderIter->size());
                                m_ppHeaderList[headerIndex][columnHeaderIter->size()] = '\0';
                                headerIndex++;
                            }
                        }
                    }
                }
                else
                {
                    columnData.clear();
                    StringUtils::Split(columnData, lineBuffer, listSeparator);

                    OccupancyInfoDataHandler* pTmpOccupancyInfo = new OccupancyInfoDataHandler();
                    pTmpOccupancyInfo->m_occupancyInfo.m_nTID = ColumnStrOutToUint(OCCUPANCY_COLUMN_NAME_THREADID);
                    pTmpOccupancyInfo->m_occupancyInfo.m_strKernelName = columnData[GetColumnIndex(OCCUPANCY_COLUMN_NAME_KERNELNAME)];
                    pTmpOccupancyInfo->m_occupancyInfo.m_strDeviceName = columnData[GetColumnIndex(OCCUPANCY_COLUMN_NAME_DEVICENAME)];
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

                    if (pTmpOccupancyInfo->m_occupancyInfo.m_nDeviceGfxIpVer == 0)
                    {
                        // TODO: include device PCIE ID in occupancy file, then we can query HW Gen using GetHardwareGeneration(deviceID, Gen)
                        AMDTDeviceInfoUtils::Instance()->GetHardwareGeneration(pTmpOccupancyInfo->m_occupancyInfo.m_strDeviceName.c_str(), pTmpOccupancyInfo->m_occupancyInfo.m_gen);
                    }
                    else
                    {
                        AMDTDeviceInfoUtils::Instance()->GfxIPVerToHwGeneration(pTmpOccupancyInfo->m_occupancyInfo.m_nDeviceGfxIpVer, pTmpOccupancyInfo->m_occupancyInfo.m_gen);
                    }

                    m_occupancyInfoList.push_back(pTmpOccupancyInfo);
                    AtpDataHandler::Instance()->OnParserProgress("Parsing Occupancy Data", static_cast<unsigned int>(currentKernelCount), static_cast<unsigned int>(kernelCount));
                    currentKernelCount++;
                }
            }

        }

        success = foundVersion;
    }

    return success;
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

void OccupancyFileInfoDataHandler::GetHeaderInOrder(char** ppColumnNames, unsigned int& columnCount) const
{
    if (nullptr != ppColumnNames && nullptr != m_ppHeaderList)
    {
        columnCount = static_cast<unsigned int>(m_headerColumnCount);
        *ppColumnNames = *m_ppHeaderList;
    }
}

void OccupancyFileInfoDataHandler::GetOccupancyThreads(osThreadId** ppThreadId, unsigned int& threadCount) const
{
    if (!m_kernelCountInfoByThreadId.empty())
    {
        threadCount = static_cast<unsigned int>(m_kernelCountInfoByThreadId.size());
        *ppThreadId = new(std::nothrow) osThreadId[threadCount];

        if (nullptr != *ppThreadId)
        {
            m_threadIdList.push_back(*ppThreadId);
            unsigned int threadIndex = 0;

            for (OccupancyInfoByThreadId::const_iterator it = m_kernelCountInfoByThreadId.begin(); it != m_kernelCountInfoByThreadId.end(); ++it)
            {
                (*ppThreadId)[threadIndex] = it->first;
                threadIndex++;
            }
        }
    }
    else
    {
        threadCount = 0u;
    }
}

void OccupancyFileInfoDataHandler::GetKernelCountByThreadId(osThreadId threadId, unsigned int& kernelCount) const
{
    OccupancyInfoByThreadId::const_iterator it = m_kernelCountInfoByThreadId.find(threadId);

    if (it != m_kernelCountInfoByThreadId.end())
    {
        kernelCount = static_cast<unsigned int>(it->second.size());
    }
}

const IOccupancyInfoDataHandler* OccupancyFileInfoDataHandler::GetOccupancyInfoDataHandler(osThreadId threadId, unsigned int index) const
{
    IOccupancyInfoDataHandler* pRetHandler = nullptr;
    OccupancyInfoByThreadId::const_iterator occupancyInfoIter = m_kernelCountInfoByThreadId.find(threadId);

    if (occupancyInfoIter != m_kernelCountInfoByThreadId.end() && index < occupancyInfoIter->second.size())
    {
        pRetHandler = occupancyInfoIter->second[index];
    }

    return pRetHandler;
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

    for (unsigned int i = 0; i < m_headerColumnCount; i++)
    {
        delete[] m_ppHeaderList[i];
    }

    delete[] m_ppHeaderList;

    for (std::vector<osThreadId*>::iterator it = m_threadIdList.begin(); it != m_threadIdList.end(); ++it)
    {
        delete(*it);
    }

    m_threadIdList.clear();
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

        for (unsigned int i = 0; i < m_headerColumnCount; i++)
        {
            delete[] m_ppHeaderList[i];
        }

        delete[] m_ppHeaderList;

        for (std::vector<osThreadId*>::iterator it = m_threadIdList.begin(); it != m_threadIdList.end(); ++it)
        {
            delete(*it);
        }

        m_threadIdList.clear();
    }
}

void OccupancyFileInfoDataHandler::GenerateKernelInfoByThreadId()
{
    for (std::vector<IOccupancyInfoDataHandler*>::iterator it = m_occupancyInfoList.begin(); it != m_occupancyInfoList.end(); ++it)
    {
        OccupancyInfoByThreadId::iterator occupancyInfoByThreadIter = m_kernelCountInfoByThreadId.find((*it)->GetThreadId());

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

