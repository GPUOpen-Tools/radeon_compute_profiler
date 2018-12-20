//==============================================================================
// Copyright (c) 2016-2018 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  This file provides the HSA Memory Summarizer
//==============================================================================

#ifndef _HSA_MEM_SUMMARIZER_H_
#define _HSA_MEM_SUMMARIZER_H_

#include <set>
#include "../HSAFdnTrace/HSAAPIInfo.h"
#include "../Common/IParserListener.h"
#include "../Common/OSUtils.h"

//------------------------------------------------------------------------------------
/// Mem Duration compare
//------------------------------------------------------------------------------------
struct HSAMemDurationCmp
{
    bool operator()(const HSAMemoryTransferAPIInfo* left, const HSAMemoryTransferAPIInfo* right)
    {
        uint64_t durationLeft = left->m_transferEndTime - left->m_transferStartTime;
        uint64_t durationRight = right->m_transferEndTime - right->m_transferStartTime;
        return durationLeft < durationRight;
    }
};

//------------------------------------------------------------------------------------
/// Memory operation summarizer
//------------------------------------------------------------------------------------
class HSAMemSummarizer
    : public IParserListener<HSAAPIInfo>
{
public:
    /// Constructor
    HSAMemSummarizer(void);

    /// Destructor
    ~HSAMemSummarizer(void);

    /// Listener function
    /// \param pAPIInfo API Info object
    /// \param[out] stopParsing flag indicating if parsing should stop after this item
    void OnParse(HSAAPIInfo* pAPIInfo, bool& stopParsing);

    /// Generate Top X data transfer HTML table from statistic data and write to std::ostream
    /// \param sout output stream
    /// \param useTopX flag indicating if the table should include all data transfers (false) or just the top X (true)
    void GenerateTopXHTMLTable(std::ostream& sout, bool useTopX);

    /// Generate HTML table from statistic data and write to std::ostream
    /// \param sout output stream
    void GenerateDataTransferSummaryHTMLTable(std::ostream& sout);

    /// Generate simple HTML page with Top X data transfers
    /// \param szFileName file name
    /// \param useTopX flag indicating if the table should include all data transfers (false) or just the top X (true)
    /// \return true if the page was generated, false otherwise
    bool GenerateTopXDataTransferHTMLPage(const char* szFileName, bool useTopX);

    /// Generate simple HTML page with list of data transfers
    /// \param szFileName file name
    /// \return true if the page was generated, false otherwise
    bool GenerateDataTransferHTMLPage(const char* szFileName);

    /// Debug
    void Debug();

protected:
    std::multiset<HSAMemoryTransferAPIInfo*, HSAMemDurationCmp> m_memAPISet; ///< memory api map, sorted by duration
    unsigned int m_uiTopX;                                                   ///< Number of top items to list

private:
    /// Copy constructor
    /// \param obj object
    HSAMemSummarizer(const HSAMemSummarizer& obj);

    /// Assignment operator
    /// \param obj object
    /// \return ref to itself
    const HSAMemSummarizer& operator = (const HSAMemSummarizer& obj);

    /// Splits the agent handle and agent name out of an agent string in an API parameter
    /// \param[in] agentString the input agent string
    /// \param[out] agentHandle the output agent handle
    /// \param[out] agentName the output agent name
    void SplitAgentHandleAndName(const std::string& agentString, std::string& agentHandle, std::string& agentName);
};

#endif //_HSA_MEM_SUMMARIZER_H_
