//==============================================================================
// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Occupancy File Info Data Handler Interface header
//==============================================================================

#ifndef _I_OCCUPANCY_FILE_INFO_DATA_HANDLER_H_
#define _I_OCCUPANCY_FILE_INFO_DATA_HANDLER_H_

// std
#include <vector>
#include <map>

// common
#include <AMDTOSWrappers/Include/osOSDefinitions.h>

#include "IOccupancyInfoDataHandler.h"

typedef std::string ColumnName;
typedef unsigned int KernelCount;

/// Occupancy File Info Data Handler
class IOccupancyFileInfoDataHandler
{
public:

    /// Parse the Occupancy file
    /// \param[in] occupancyFile name of the occupancy file
    /// \return flag indicating the parsing was successful or not
    virtual bool ParseOccupancyFile(const std::string& occupancyFile) = 0;

    /// Indicates the data is ready or not after parsing
    /// \return flag indicating the parsed data is ready for use or not
    virtual bool IsDataReady() const = 0;

    /// Gets the occupancy file version
    /// \param[out] major major version of the occupancy file format
    /// \param[out] minor minor version of the occupancy file format
    virtual void GetOccupancyFileVersion(unsigned int& major, unsigned int& minor) const = 0;

    /// Returns the list of the column names of the header of the occupancy file
    /// \return list containing the header data of the occupancy file
    virtual std::vector<ColumnName> GetHeaderInOrder() const = 0;

    /// Returns the map of count of the kernels associated with a thread
    /// \return map containing the count of the kernels based on thread Id
    virtual std::map<osThreadId, KernelCount> GetKernelCountByThreadId() const = 0;

    /// Get the occupancy info interface pointer associated with a thread and index of kernel on that thread
    /// \return Occupancy info interface pointer
    virtual const IOccupancyInfoDataHandler* GetOccupancyInfoDataHandler(osThreadId, unsigned int index) const = 0;

    /// Gets the list of all the kernel occupancy info on a thread
    /// \return list of the occupany info interface pointer
    virtual std::vector<const IOccupancyInfoDataHandler*> GetOccupancyInfoByThreadId(osThreadId threadId) const = 0;

    /// Releases the data and frees the memory
    virtual void ReleaseData() = 0;

    /// Virtual Destructor
    virtual ~IOccupancyFileInfoDataHandler() = default;
};

#endif // _I_OCCUPANCY_FILE_INFO_DATA_HANDLER_H_
