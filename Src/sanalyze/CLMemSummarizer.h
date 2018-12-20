//==============================================================================
// Copyright (c) 2015 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  This file provides the CL Memory Summarizer
//==============================================================================

#ifndef _CL_MEM_SUMMARIZER_H_
#define _CL_MEM_SUMMARIZER_H_

#include <set>
#include "../CLTraceAgent/CLAPIInfo.h"
#include "../Common/IParserListener.h"
#include "../Common/OSUtils.h"

//------------------------------------------------------------------------------------
/// Mem Duration compare
//------------------------------------------------------------------------------------
struct CLMemDurationCmp
{
    bool operator()(const CLMemAPIInfo* left, const CLMemAPIInfo* right)
    {
        ULONGLONG ullDurationLeft = left->m_ullComplete - left->m_ullRunning;
        ULONGLONG ullDurationRight = right->m_ullComplete - right->m_ullRunning;
        return ullDurationLeft < ullDurationRight;
    }
};

//------------------------------------------------------------------------------------
/// Memory operation summarizer
//------------------------------------------------------------------------------------
class CLMemSummarizer
    : public IParserListener<CLAPIInfo>
{
public:
    /// Constructor
    CLMemSummarizer(void);

    /// Destructor
    ~CLMemSummarizer(void);

    /// Listener function
    /// \param pAPIInfo API Info object
    /// \param[out] stopParsing flag indicating if parsing should stop after this item
    void OnParse(CLAPIInfo* pAPIInfo, bool& stopParsing);

    /// Generate Top X data transfer HTML table from statistic data and write to std::ostream
    /// \param sout output stream
    /// \param useTopX flag indicating if the table should include all data transfers (false) or just the top X (true)
    void GenerateTopXHTMLTable(std::ostream& sout, bool useTopX);

    /// Generate HTML table from statistic data and write to std::ostream
    /// \param sout output stream
    void GenerateDataTransferSummaryHTMLTable(std::ostream& sout);

    /// Generate simple HTML page with list of data transfers
    /// \param szFileName file name
    /// \return true if the page was generated, false otherwise
    bool GenerateDataTransferHTMLPage(const char* szFileName);

    /// Generate simple HTML page with Top X data transfers
    /// \param szFileName file name
    /// \param useTopX flag indicating if the table should include all data transfers (false) or just the top X (true)
    /// \return true if the page was generated, false otherwise
    bool GenerateTopXDataTransferHTMLPage(const char* szFileName, bool useTopX);

    /// Debug
    void Debug();

protected:
    std::multiset<CLMemAPIInfo*, CLMemDurationCmp> m_memAPISet; /// memory api map, sorted by duration
    unsigned int m_uiTopX;           ///< Number of top items to list
private:
    /// Copy constructor
    /// \param obj object
    CLMemSummarizer(const CLMemSummarizer& obj);

    /// Assignment operator
    /// \param obj object
    /// \return ref to itself
    const CLMemSummarizer& operator = (const CLMemSummarizer& obj);
};

#endif //_CL_MEM_SUMMARIZER_H_
