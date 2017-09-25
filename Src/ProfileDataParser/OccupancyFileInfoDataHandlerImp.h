//==============================================================================
// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Occupancy File Info Data Handler Interface Implementation header
//==============================================================================

#ifndef _OCCUPANCY_FILE_INFO_DATA_HANDLER_IMP_H_
#define _OCCUPANCY_FILE_INFO_DATA_HANDLER_IMP_H_

// profiler common
#include "IOccupancyFileInfoDataHandler.h"

typedef std::map<osThreadId, std::vector<IOccupancyInfoDataHandler*>> OccupancyInfoByThreadId;

/// Occupancy file info data handler class
class OccupancyFileInfoDataHandler : public IOccupancyFileInfoDataHandler
{
public:

    /// Constructor
    /// \param[in] occupancyFileName occupancy file name
    OccupancyFileInfoDataHandler(std::string& occupancyFileName);

    /// Parse the Occupancy file
    /// \param[in] pOccupancyFile name of the occupancy file
    /// \return flag indicating the parsing was successful or not
    bool ParseOccupancyFile(const char* pOccupancyFile) override;

    /// Indicates the data is ready or not after parsing
    /// \return flag indicating the parsed data is ready for use or not
    bool IsDataReady() const override;

    /// Gets the occupancy file version
    /// \param[out] major major version of the occupancy file format
    /// \param[out] minor minor version of the occupancy file format
    void GetOccupancyFileVersion(unsigned int& major, unsigned int& minor) const override;

    /// Gets the list of the column names of the header of the occupancy file
    /// \param[out] ppColumnNames containing the header data of the occupancy file
    /// \param[out] columnCount number of the columns in the occupancy file
    void GetHeaderInOrder(char** ppColumnNames, unsigned int& columnCount) const override;

    /// Gets all the distinct threads in the occupancy file
    /// \param[out] ppThreadId pointer to the array of all distinct threads
    /// \param[out] threadCount number of distinct threads in the occupancy file
    void GetOccupancyThreads(osThreadId** ppThreadId, unsigned int& threadCount) const override;

    /// Gets the kernel count of the thread ID
    /// \param[in] threadId thread Id of the kernel
    /// \param[out] kernelCount number of the kernels for the given thread id in the occupancy file
    void GetKernelCountByThreadId(osThreadId threadId, unsigned int& kernelCount) const override;

    /// Get the occupancy info interface pointer associated with a thread and index of kernel on that thread
    /// \param[in] threadId thread id of the kernel
    /// \param[in] index index of the kernel on the given thread
    /// \return Occupancy info interface pointer
    const IOccupancyInfoDataHandler* GetOccupancyInfoDataHandler(osThreadId threadId, unsigned int index) const override;

    /// Releases the data and frees the memory
    void ReleaseData() override;

    /// Virtual Destructor
    ~OccupancyFileInfoDataHandler();

private:

    /// Stores the kernel information by thread id
    void GenerateKernelInfoByThreadId();

    /// Parses the occupancy file
    /// \param[in] pOccupancyFile occupancy file name
    /// \return true if parsing is successful otherwise false
    bool Parse(const char* pOccupancyFile);

    std::string                                     m_occupancyFileName;                    ///< occupancy file name
    bool                                            m_bIsDataReady;                         ///< flag indicates the data is reday or not after parsing
    char**                                          m_ppHeaderList;                         ///< header names in the occupancy file
    unsigned int                                    m_headerColumnCount;                    ///< number of the header columns in the occupancy file
    unsigned int                                    m_occupancyFileMajorVersion;            ///< occupancy file major version
    unsigned int                                    m_occupancyFileMinorVersion;            ///< occupancy file minor version
    OccupancyInfoByThreadId                         m_kernelCountInfoByThreadId;            ///< kernels info per thread Id
    std::vector<IOccupancyInfoDataHandler*>         m_occupancyInfoList;                    ///< list of occupancy info
    mutable std::vector<osThreadId*>                m_threadIdList;                         ///< list of the thread Ids - for deallocation
};

#endif // _OCCUPANCY_FILE_INFO_DATA_HANDLER_IMP_H_
