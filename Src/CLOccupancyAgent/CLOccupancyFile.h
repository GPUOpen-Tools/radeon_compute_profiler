//==============================================================================
// Copyright (c) 2015 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Merge tmp file to occupancy File
//==============================================================================

#ifndef _CL_OCCUPANCY_FILE_H_
#define _CL_OCCUPANCY_FILE_H_

#include <string>

#include <AMDTBaseTools/Include/gtString.h>
#include "ProfilerOutputFileDefs.h"

/// Info in the occupancy file header
struct CLOccupancyHdr
{
    int m_iVersionMajor = 0; ///< Major version
    int m_iVersionMinor = 0; ///< Minor version
    gtString m_strAppName;   ///< Application name
    gtString m_strAppArgs;   ///< Application arguments
    char m_listSeparator;    ///< List separator character used
};

/// Write occupancy header
/// \param sout output stream
/// \param header Occupancy file header
/// \param kernelCount number of the kernels/occupancy
/// \param cListSeparator List separator
void WriteOccupancyHeader(
    std::ostream& sout,
    const CLOccupancyHdr& header,
    size_t kernelCount,
    char cListSeparator = OCCUPANCY_DEFAULT_LIST_SEPARATOR_CHAR);

/// Merge tmp cl trace file
/// \param [in] strOutputFile output occupancy file
/// \param [in] strTmpFilePath cl occupancy file path
/// \param [in] strFilePrefix file prefix
/// \param [in] occupancyHeader file header
/// \return true if succeed
bool MergeTmpCLOccupancyFile(const std::string& strOutputFile,
                             const gtString& strTmpFilePath,
                             const std::string& strFilePrefix,
                             const CLOccupancyHdr& occupancyHeader);

#endif
