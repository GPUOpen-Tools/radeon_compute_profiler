//==============================================================================
// Copyright (c) 2015 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Merge tmp file to occupancy File
//==============================================================================

#include <string>
#include <sstream>
#include <fstream>
#include <iterator>
#include <iostream>
#include "CLOccupancyFile.h"
#include "../Common/FileUtils.h"
#include "../Common/LocaleSetting.h"
#include "../Common/StringUtils.h"

#include <AMDTBaseTools/Include/gtString.h>
#include "ProfilerOutputFileDefs.h"

using namespace std;

void WriteOccupancyHeader(std::ostream& sout, const CLOccupancyHdr& occupancyHeader, size_t kernelCount, char cListSeparator)
{
    string convertedInfo;
    sout << OCCUPANCY_HEADER_START << FILE_HEADER_PROFILER_VERSION << EQUAL_SIGN_STR << RCP_MAJOR_VERSION << "." << RCP_MINOR_VERSION << "." << RCP_BUILD_NUMBER << std::endl;
    StringUtils::WideStringToUtf8String(occupancyHeader.m_strAppName.asCharArray(), convertedInfo);
    sout << OCCUPANCY_HEADER_START << FILE_HEADER_APPLICATION << EQUAL_SIGN_STR << convertedInfo.c_str() << std::endl;
    StringUtils::WideStringToUtf8String(occupancyHeader.m_strAppArgs.asCharArray(), convertedInfo);
    sout << OCCUPANCY_HEADER_START << FILE_HEADER_APPLICATION_ARGS << EQUAL_SIGN_STR << convertedInfo.c_str() << std::endl;
    sout << OCCUPANCY_HEADER_START << FILE_HEADER_LIST_SEPARATOR << EQUAL_SIGN_STR << occupancyHeader.m_listSeparator << std::endl;
    sout << OCCUPANCY_HEADER_START << FILE_HEADER_KERNEL_COUNT << EQUAL_SIGN_STR << kernelCount << std::endl;

    sout << left << OCCUPANCY_COLUMN_NAME_THREADID << cListSeparator;
    sout << left << OCCUPANCY_COLUMN_NAME_KERNELNAME << cListSeparator;
    sout << left << OCCUPANCY_COLUMN_NAME_DEVICENAME << cListSeparator;
    sout << left << OCCUPANCY_COLUMN_NAME_NUMBEROFCOMPUTEUNITS << cListSeparator;
    sout << left << OCCUPANCY_COLUMN_NAME_MAXNUMBEROFWAVEFRONTSPERCU << cListSeparator;
    sout << left << OCCUPANCY_COLUMN_NAME_MAXNUMBEROFWORKGROUPPERCU << cListSeparator;
    sout << left << OCCUPANCY_COLUMN_NAME_MAXNUMBEROFVGPR << cListSeparator;
    sout << left << OCCUPANCY_COLUMN_NAME_MAXNUMBEROFSGPR << cListSeparator;
    sout << left << OCCUPANCY_COLUMN_NAME_MAXAMOUNTOFLDS << cListSeparator;
    sout << left << OCCUPANCY_COLUMN_NAME_NUMBEROFVGPRUSED << cListSeparator;
    sout << left << OCCUPANCY_COLUMN_NAME_NUMBEROFSGPRUSED << cListSeparator;
    sout << left << OCCUPANCY_COLUMN_NAME_AMOUNTOFLDSUSED << cListSeparator;
    sout << left << OCCUPANCY_COLUMN_NAME_SIZEOFWAVEFRONT << cListSeparator;
    sout << left << OCCUPANCY_COLUMN_NAME_WORKGROUPSIZE << cListSeparator;
    sout << left << OCCUPANCY_COLUMN_NAME_WAVEFRONTSPERWORKGROUP << cListSeparator;
    sout << left << OCCUPANCY_COLUMN_NAME_MAXWORKGROUPSIZE << cListSeparator;
    sout << left << OCCUPANCY_COLUMN_NAME_MAXWAVEFRONTSPERWORKGROUP << cListSeparator;
    sout << left << OCCUPANCY_COLUMN_NAME_GLOBALWORKSIZE << cListSeparator;
    sout << left << OCCUPANCY_COLUMN_NAME_MAXIMUMGLOBALWORKSIZE << cListSeparator;
    sout << left << OCCUPANCY_COLUMN_NAME_NBRVGPRLIMITEDWAVES << cListSeparator;
    sout << left << OCCUPANCY_COLUMN_NAME_NBRSGPRLIMITEDWAVES << cListSeparator;
    sout << left << OCCUPANCY_COLUMN_NAME_NBRLDSLIMITEDWAVES << cListSeparator;
    sout << left << OCCUPANCY_COLUMN_NAME_NBROFWGLIMITEDWAVES << cListSeparator;
    sout << left << OCCUPANCY_COLUMN_NAME_KERNELOCCUPANCY << cListSeparator;
    sout << left << OCCUPANCY_COLUMN_NAME_GRAPHICSIPVERSION << cListSeparator;
    sout << left << OCCUPANCY_COLUMN_NAME_SIMDSPERCU;
}

bool MergeTmpCLOccupancyFile(const std::string& strOutputFile,
                             const gtString& strTmpFilePath,
                             const std::string& strFilePrefix,
                             const CLOccupancyHdr& occupancyHeader)
{
    bool bStatus = true;

    // Check that the file extension is valid for the occupancy output file
    string strExtension("");
    string strOccupancyFile;

    strExtension = FileUtils::GetFileExtension(strOutputFile);

    if (strExtension != OCCUPANCY_EXT)
    {
        if ((strExtension == TRACE_EXT) || (strExtension == PERF_COUNTER_EXT))
        {
            string strBaseFileName = FileUtils::GetBaseFileName(strOutputFile);
            strOccupancyFile = strBaseFileName + "." + OCCUPANCY_EXT;
        }
        else
        {
            strOccupancyFile = strOutputFile + "." + OCCUPANCY_EXT;
        }
    }
    else
    {
        strOccupancyFile = strOutputFile;
    }

    wstring strUnicodePrefix;
    wstring strUnicodeExt;
    StringUtils::Utf8StringToWideString(strFilePrefix, strUnicodePrefix);
    StringUtils::Utf8StringToWideString(TMP_OCCUPANCY_EXT, strUnicodeExt);

    gtList<osFilePath> files;
    bool ret = FileUtils::GetTmpFilesToMerge(strTmpFilePath, strUnicodePrefix.c_str(), strUnicodeExt.c_str(), files);

    // Count the kernels in the map
    size_t kernelCount = 0u;

    if (ret)
    {
        std::wstring strFullFilePath;
        std::string strFileContent;
        osFilePath strFileAsFilePath;
        gtString strFileName;
        std::wstring strFile;

        for (gtList<osFilePath>::iterator it = files.begin(); it != files.end(); ++it)
        {
            strFileAsFilePath = (*it);
            strFileAsFilePath.getFileNameAndExtension(strFileName);
            strFile = strFileName.asCharArray();
            strFullFilePath = strFileAsFilePath.asString().asCharArray();

            if (FileUtils::ReadFile(strFullFilePath, strFileContent))
            {
                kernelCount += StringUtils::GetNumLines(strFileContent);
            }
        }
    }

    std::stringstream headerStream;
    char separator = LocaleSetting::GetListSeparator();
    WriteOccupancyHeader(headerStream, occupancyHeader, kernelCount, separator);

    FileUtils::MergeTmpTraceFiles(strOccupancyFile, strTmpFilePath, strUnicodePrefix.c_str(), strUnicodeExt.c_str(), headerStream.str().c_str(), FileUtils::MergeSummaryType_None);

    return bStatus;
}
