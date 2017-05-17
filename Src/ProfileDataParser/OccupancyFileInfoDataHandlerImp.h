//==============================================================================
// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Occupancy File Info Data Handler Interface Implementation header
//==============================================================================

#ifndef _OCCUPANCY_FILE_INFO_DATA_HANDLER_IMP_H_
#define _OCCUPANCY_FILE_INFO_DATA_HANDLER_IMP_H_

// profiler common
#include <IParserListener.h>
#include <CSVFileParser.h>

#include "IOccupancyFileInfoDataHandler.h"

typedef std::map<osThreadId, std::vector<IOccupancyInfoDataHandler*>> OccupancyInfoByThreadId;

/// Occupancy file info data handler class
class OccupancyFileInfoDataHandler : public IOccupancyFileInfoDataHandler,
    public IParserListener<CSVRow>
{
public:

    /// Constructor
    /// \param[in] occupancyFileName occupancy file name
    OccupancyFileInfoDataHandler(std::string& occupancyFileName);

    /// Parse the Occupancy file
    /// \param[in] occupancyFile name of the occupancy file
    /// \return flag indicating the parsing was successful or not
    bool ParseOccupancyFile(const std::string& occupancyFile) override;

    /// Indicates the data is ready or not after parsing
    /// \return flag indicating the parsed data is ready for use or not
    bool IsDataReady() const override;

    /// Gets the occupancy file version
    /// \param[out] major major version of the occupancy file format
    /// \param[out] minor minor version of the occupancy file format
    void GetOccupancyFileVersion(unsigned int& major, unsigned int& minor) const override;

    /// Returns the list of the column names of the header of the occupancy file
    /// \return list containing the header data of the occupancy file
    std::vector<ColumnName> GetHeaderInOrder() const override;

    /// Returns the map  of count of the kernels associated with a thread
    /// \return map containig the count of the kernels based on thread Id
    std::map<osThreadId, KernelCount> GetKernelCountByThreadId() const override;

    /// Get the occupancy info interface pointer associated with a thread and index of kernel on that thread
    /// param[in] threadId thread Id
    /// param[in] index kernel index
    /// \return occupancy info interface pointer
    const IOccupancyInfoDataHandler* GetOccupancyInfoDataHandler(osThreadId threadId, unsigned int index) const override;

    /// Gets the list of all the kernel occupancy info on a thread
    /// \param[in] threadId thread id
    /// \return list of the occupancy info interface pointer
    std::vector<const IOccupancyInfoDataHandler*> GetOccupancyInfoByThreadId(osThreadId threadId) const override;

    /// Releases the data and frees the memory
    void ReleaseData() override;

    /// Override function for CSV Parser Listener
    /// \param[in] pCsvRow pointer to csv row
    /// \param[in,out] stopParsing flag indicating to stop parsing or not
    void OnParse(CSVRow* pCsvRow, bool& stopParsing) override;

    /// Virtual Destructor
    ~OccupancyFileInfoDataHandler();

private:

    /// Stores the kernel information by thread id
    void GenerateKernelInfoByThreadId();

    std::string                                     m_occupancyFileName;                    ///< occupancy file name
    bool                                            m_bIsDataReady;                         ///< flag indicates the data is reday or not after parsing
    std::vector<ColumnName>                         m_headerList;                           ///< header names in the occupancy file
    unsigned int                                    m_occupancyFileMajorVersion;            ///< occupancy file major version
    unsigned int                                    m_occupancyFileMinorVersion;            ///< occupancy file minor version
    OccupancyInfoByThreadId                         m_kernelCountInfoByThreadId;            ///< kernels info per thread Id
    std::vector<IOccupancyInfoDataHandler*>         m_occupancyInfoList;                    ///< list of occupancy info
};

#endif // _OCCUPANCY_FILE_INFO_DATA_HANDLER_IMP_H_
