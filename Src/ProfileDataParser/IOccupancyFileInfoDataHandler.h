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

/// Occupancy File Info Data Handler
class IOccupancyFileInfoDataHandler
{
public:

    /// Parse the Occupancy file
    /// \param[in] pOccupancyFile name of the occupancy file
    /// \return flag indicating the parsing was successful or not
    virtual bool ParseOccupancyFile(const char* pOccupancyFile) = 0;

    /// Indicates the data is ready or not after parsing
    /// \return flag indicating the parsed data is ready for use or not
    virtual bool IsDataReady() const = 0;

    /// Gets the occupancy file version
    /// \param[out] major major version of the occupancy file format
    /// \param[out] minor minor version of the occupancy file format
    virtual void GetOccupancyFileVersion(unsigned int& major, unsigned int& minor) const = 0;

    /// Gets the list of the column names of the header of the occupancy file
    /// \param[out] ppColumnNames containing the header data of the occupancy file
    /// \param[out] columnCount number of the columns in the occupancy file
    virtual void GetHeaderInOrder(char** ppColumnNames, unsigned int& columnCount) const = 0;

    /// Gets all the distinct threads in the occupancy file
    /// \param[out] ppThreadId pointer to the array of all distinct threads
    /// \param[out] threadCount number of distinct threads in the occupancy file
    virtual void GetOccupancyThreads(osThreadId** ppThreadId, unsigned int& threadCount) const = 0;

    /// Gets the kernel count of the thread ID
    /// \param[in] threadId thread Id of the kernel
    /// \param[out] kernelCount number of the kernels for the given thread id in the occupancy file
    virtual void GetKernelCountByThreadId(osThreadId threadId, unsigned int& kernelCount) const = 0;

    /// Get the occupancy info interface pointer associated with a thread and index of kernel on that thread
    /// \param[in] threadId thread id of the kernel
    /// \param[in] index index of the kernel on the given thread
    /// \return Occupancy info interface pointer
    virtual const IOccupancyInfoDataHandler* GetOccupancyInfoDataHandler(osThreadId threadId, unsigned int index) const = 0;

    /// Releases the data and frees the memory
    virtual void ReleaseData() = 0;

    /// Virtual Destructor
    virtual ~IOccupancyFileInfoDataHandler() {};
};

#endif // _I_OCCUPANCY_FILE_INFO_DATA_HANDLER_H_
