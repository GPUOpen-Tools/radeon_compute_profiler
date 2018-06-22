//==============================================================================
// Copyright (c) 2015 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief File utils functions specifically for occupancy files
//==============================================================================

// std
#include <fstream>
#include <iostream>
#include <string>

// common
#include <DeviceInfoUtils.h>

// profiler common
#include <StringUtils.h>

#include "OccupancyUtils.h"
#include "../sanalyze/KernelProfileResultParser.h"
#include "../CLOccupancyAgent/CLOccupancyFile.h"

using std::ifstream;

#define CHECKPARAM(param) \
    if (!b##param##Found) \
    { \
        strError = "Parameter \"" #param "\" not found in input file: " + strOccupancyFile; \
        return false; \
    }

#define PARSEPARAM(param, var) \
    if (strKey == #param) \
    { \
        if (StringUtils::Parse(strValue, params.var) == false) \
        { \
            strError = "Invalid value for \"" #param "\" in input file: " + strOccupancyFile; \
            return false; \
        } \
        b##param##Found = true; \
    }

#define OPTIONALPARSEPARAM(param, var, defaultval) \
    if (strKey == #param) \
    { \
        if (StringUtils::Parse(strValue, params.var) == false) \
        { \
            params.var = defaultval; \
        } \
        b##param##Found = true; \
    }

#define PARSEPARAM2(param, var) \
    if (!results[occupancyIndex]->GetValue(0, param, strValue)) \
    { \
        strError = "Parameter \"" param "\" not found in input file: " + strOccupancyFile; \
        return false; \
    } \
    else \
    { \
        if (!StringUtils::Parse(strValue, params.var)) \
        { \
            strError = "Invalid value for \"" param "\" in input file: " + strOccupancyFile; \
            return false; \
        } \
    }

#define OPTIONALPARSEPARAM2(param, var, defaultval) \
    if (!results[occupancyIndex]->GetValue(0, param, strValue)) \
    { \
        strValue = defaultval; \
    } \
    \
    if (!StringUtils::Parse(strValue, params.var)) \
    { \
        strError = "Invalid value for \"" param "\" in input file: " + strOccupancyFile; \
        return false; \
    }

const unsigned int MAX_PATH_LENGTH = 512;

bool OccupancyUtils::GetOccupancyParamsFromFile(const std::string& strOccupancyFile, OccupancyParams& params, std::string& strError)
{
    ifstream fin(strOccupancyFile.c_str());

    if (!fin.is_open())
    {
        strError = "Unable to open occupancy parameters file";
        return false;
    }

    char szBuffer[ MAX_PATH_LENGTH ];
    std::string strBuffer;
    std::string strKey;
    std::string strValue;
    size_t nIdx = 0;

    bool bThreadIDFound = false;
    bool bCallIndexFound = false;
    bool bKernelNameFound = false;
    bool bDeviceNameFound = false;
    bool bComputeUnitsFound = false;
    bool bMaxWavesPerComputeUnitFound = false;
    bool bMaxWorkGroupPerComputeUnitFound = false;
    bool bMaxVGPRsFound = false;
    bool bMaxSGPRsFound = false;
    bool bMaxLDSFound = false;
    bool bUsedVGPRsFound = false;
    bool bUsedSGPRsFound = false;
    bool bUsedLDSFound = false;
    bool bWavefrontSizeFound = false;
    bool bWorkGroupSizeFound = false;
    bool bWavesPerWorkGroupFound = false;
    bool bMaxWorkGroupSizeFound = false;
    bool bMaxWavesPerWorkGroupFound = false;
    bool bGlobalWorkSizeFound = false;
    bool bMaxGlobalWorkSizeFound = false;
    bool bWavesLimitedByVGPRFound = false;
    bool bWavesLimitedBySGPRFound = false;
    bool bWavesLimitedByLDSFound = false;
    bool bWavesLimitedByWorkgroupFound = false;
    bool bOccupancyFound = false;
    bool bDeviceGfxIpVerFound = false;
    bool bSimdsPerCUFound = false;

    while (!fin.eof())
    {
        fin.getline(szBuffer, MAX_PATH_LENGTH);
        strBuffer.assign(szBuffer);
        nIdx = strBuffer.find("=");
        strKey = strBuffer.substr(0, nIdx);
        strValue = strBuffer.substr(nIdx + 1);

        PARSEPARAM(ThreadID, m_nTID);
        PARSEPARAM(CallIndex, m_nCallIndex);

        if (strKey.find("KernelName") != std::string::npos)
        {
            params.m_strKernelName = strValue;
            bKernelNameFound = true;
        }

        if (strKey.find("DeviceName") != std::string::npos)
        {
            params.m_strDeviceName = strValue;
            bDeviceNameFound = true;
        }

        PARSEPARAM(ComputeUnits, m_nNbrComputeUnits);
        PARSEPARAM(MaxWavesPerComputeUnit, m_nMaxWavesPerCU);
        PARSEPARAM(MaxWorkGroupPerComputeUnit, m_nMaxWGPerCU);
        PARSEPARAM(MaxVGPRs, m_nMaxVGPRS);
        PARSEPARAM(MaxSGPRs, m_nMaxSGPRS);
        PARSEPARAM(MaxLDS, m_nMaxLDS);
        PARSEPARAM(UsedVGPRs, m_nUsedVGPRS);
        PARSEPARAM(UsedSGPRs, m_nUsedSGPRS);
        PARSEPARAM(UsedLDS, m_nUsedLDS);
        PARSEPARAM(WavefrontSize, m_nWavefrontSize);
        PARSEPARAM(WorkGroupSize, m_nWorkgroupSize);
        PARSEPARAM(WavesPerWorkGroup, m_nWavesPerWG);
        PARSEPARAM(MaxWorkGroupSize, m_nMaxWGSize);
        PARSEPARAM(MaxWavesPerWorkGroup, m_nMaxWavesPerWG);
        PARSEPARAM(GlobalWorkSize, m_nGlobalWorkSize);
        PARSEPARAM(MaxGlobalWorkSize, m_nMaxGlobalWorkSize);
        PARSEPARAM(WavesLimitedByVGPR, m_nVGPRLimitedWaveCount);
        PARSEPARAM(WavesLimitedBySGPR, m_nSGPRLimitedWaveCount);
        PARSEPARAM(WavesLimitedByLDS, m_nLDSLimitedWaveCount);
        PARSEPARAM(WavesLimitedByWorkgroup, m_nWGLimitedWaveCount);
        PARSEPARAM(Occupancy, m_fOccupancy);
        PARSEPARAM(DeviceGfxIpVer, m_nDeviceGfxIpVer);
        OPTIONALPARSEPARAM(SimdsPerCU, m_nSimdsPerCU, 4); // use 4 if the occupancy file does not have this value (older files may not have this)
    }

    CHECKPARAM(ThreadID);
    CHECKPARAM(CallIndex);
    CHECKPARAM(KernelName);
    CHECKPARAM(DeviceName);
    CHECKPARAM(ComputeUnits);
    CHECKPARAM(MaxWavesPerComputeUnit);
    CHECKPARAM(MaxWorkGroupPerComputeUnit);
    CHECKPARAM(MaxVGPRs);
    CHECKPARAM(MaxSGPRs);
    CHECKPARAM(MaxLDS);
    CHECKPARAM(UsedVGPRs);
    CHECKPARAM(UsedSGPRs);
    CHECKPARAM(UsedLDS);
    CHECKPARAM(WavefrontSize);
    CHECKPARAM(WorkGroupSize);
    CHECKPARAM(WavesPerWorkGroup);
    CHECKPARAM(MaxWorkGroupSize);
    CHECKPARAM(MaxWavesPerWorkGroup);
    CHECKPARAM(GlobalWorkSize);
    CHECKPARAM(MaxGlobalWorkSize);
    CHECKPARAM(WavesLimitedByVGPR);
    CHECKPARAM(WavesLimitedBySGPR);
    CHECKPARAM(WavesLimitedByLDS);
    CHECKPARAM(WavesLimitedByWorkgroup);
    CHECKPARAM(Occupancy);
    CHECKPARAM(DeviceGfxIpVer);
    CHECKPARAM(SimdsPerCU);

    if (!AMDTDeviceInfoUtils::Instance()->GfxIPVerToHwGeneration(params.m_nDeviceGfxIpVer, params.m_gen))
    {
        return false;
    }

    strError.clear();
    return true;
}

bool OccupancyUtils::GetOccupancyParamsFromFile(const std::string& strOccupancyFile, unsigned int occupancyIndex, OccupancyParams& params, std::string& strError)
{
    SP_UNREFERENCED_PARAMETER(params);
    SP_UNREFERENCED_PARAMETER(occupancyIndex);

    KernelProfileResultParser occupancyFile;

    if (occupancyFile.Parse(strOccupancyFile))
    {
        const KernelProfileResultList& results = occupancyFile.GetProfileResults();

        if (occupancyIndex < results.size())
        {
            if (!results[occupancyIndex]->GetValue(0, OCCUPANCY_COLUMN_NAME_KERNELNAME, params.m_strKernelName))
            {
                strError = "\"" OCCUPANCY_COLUMN_NAME_KERNELNAME "\" not found in input file: " + strOccupancyFile;
                return false;
            }

            if (!results[occupancyIndex]->GetValue(0, OCCUPANCY_COLUMN_NAME_DEVICENAME, params.m_strDeviceName))
            {
                strError = "\"" OCCUPANCY_COLUMN_NAME_DEVICENAME "\" not found in input file: " + strOccupancyFile;
                return false;
            }

            std::string strValue;

            PARSEPARAM2(OCCUPANCY_COLUMN_NAME_THREADID, m_nTID);
            PARSEPARAM2(OCCUPANCY_COLUMN_NAME_NUMBEROFCOMPUTEUNITS, m_nNbrComputeUnits);
            PARSEPARAM2(OCCUPANCY_COLUMN_NAME_MAXNUMBEROFWAVEFRONTSPERCU, m_nMaxWavesPerCU);
            PARSEPARAM2(OCCUPANCY_COLUMN_NAME_MAXNUMBEROFWORKGROUPPERCU, m_nMaxWGPerCU);
            PARSEPARAM2(OCCUPANCY_COLUMN_NAME_MAXNUMBEROFVGPR, m_nMaxVGPRS);
            PARSEPARAM2(OCCUPANCY_COLUMN_NAME_MAXNUMBEROFSGPR, m_nMaxSGPRS);
            PARSEPARAM2(OCCUPANCY_COLUMN_NAME_MAXAMOUNTOFLDS, m_nMaxLDS);
            PARSEPARAM2(OCCUPANCY_COLUMN_NAME_NUMBEROFVGPRUSED, m_nUsedVGPRS);
            PARSEPARAM2(OCCUPANCY_COLUMN_NAME_NUMBEROFSGPRUSED, m_nUsedSGPRS);
            PARSEPARAM2(OCCUPANCY_COLUMN_NAME_AMOUNTOFLDSUSED, m_nUsedLDS);
            PARSEPARAM2(OCCUPANCY_COLUMN_NAME_SIZEOFWAVEFRONT, m_nWavefrontSize);
            PARSEPARAM2(OCCUPANCY_COLUMN_NAME_WORKGROUPSIZE, m_nWorkgroupSize);
            PARSEPARAM2(OCCUPANCY_COLUMN_NAME_WAVEFRONTSPERWORKGROUP, m_nWavesPerWG);
            PARSEPARAM2(OCCUPANCY_COLUMN_NAME_MAXWORKGROUPSIZE, m_nMaxWGSize);
            PARSEPARAM2(OCCUPANCY_COLUMN_NAME_MAXWAVEFRONTSPERWORKGROUP, m_nMaxWavesPerWG);
            PARSEPARAM2(OCCUPANCY_COLUMN_NAME_GLOBALWORKSIZE, m_nGlobalWorkSize);
            PARSEPARAM2(OCCUPANCY_COLUMN_NAME_MAXIMUMGLOBALWORKSIZE, m_nMaxGlobalWorkSize);
            PARSEPARAM2(OCCUPANCY_COLUMN_NAME_NBRVGPRLIMITEDWAVES, m_nVGPRLimitedWaveCount);
            PARSEPARAM2(OCCUPANCY_COLUMN_NAME_NBRSGPRLIMITEDWAVES, m_nSGPRLimitedWaveCount);
            PARSEPARAM2(OCCUPANCY_COLUMN_NAME_NBRLDSLIMITEDWAVES, m_nLDSLimitedWaveCount);
            PARSEPARAM2(OCCUPANCY_COLUMN_NAME_NBROFWGLIMITEDWAVES, m_nWGLimitedWaveCount);
            PARSEPARAM2(OCCUPANCY_COLUMN_NAME_KERNELOCCUPANCY, m_fOccupancy);
            PARSEPARAM2(OCCUPANCY_COLUMN_NAME_GRAPHICSIPVERSION, m_nDeviceGfxIpVer);
            OPTIONALPARSEPARAM2(OCCUPANCY_COLUMN_NAME_SIMDSPERCU, m_nSimdsPerCU, "4"); // use 4 if the occupancy file does not have this value (older files will not have this)

            if (!AMDTDeviceInfoUtils::Instance()->GfxIPVerToHwGeneration(params.m_nDeviceGfxIpVer, params.m_gen))
            {
                return false;
            }
        }
    }

    strError.clear();
    return true;
}
