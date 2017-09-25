//==============================================================================
// Copyright (c) 2015 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  This file provides the CL Memory Summarizer
//==============================================================================

#include <fstream>
#include <sstream>
#include "HSAMemSummarizer.h"
#include "AnalyzerHTMLUtils.h"
#include "../Common/HTMLTable.h"
#include "../Common/StringUtils.h"
#include <Logger.h>

using std::stringstream;
using std::ofstream;
using std::multiset;
using std::string;

HSAMemSummarizer::HSAMemSummarizer(void)
{
    m_uiTopX = 10;
}

HSAMemSummarizer::~HSAMemSummarizer(void)
{
}

void HSAMemSummarizer::OnParse(HSAAPIInfo* pAPIInfo, bool& stopParsing)
{
    stopParsing = false;

    if (pAPIInfo->m_apiID == HSA_API_Type_hsa_amd_memory_async_copy)
    {
        HSAMemoryTransferAPIInfo* pMemTransferAPI = dynamic_cast<HSAMemoryTransferAPIInfo*>(pAPIInfo);

        if (pMemTransferAPI->m_transferEndTime < pMemTransferAPI->m_transferStartTime)
        {
            GPULogger::Log(GPULogger::logERROR, "HSAMemSummarizer: mem transfer end time is less than start time\n");
            return;
        }

        m_memAPISet.insert(pMemTransferAPI);
    }
}

void HSAMemSummarizer::Debug()
{
}

/// Generate HTML table from statistic data and write to std::ostream
/// \param sout output stream
void HSAMemSummarizer::GenerateHTMLTable(std::ostream& sout)
{
    HTMLTable table;

    unsigned int count = 0;

    table.AddColumn("Source Agent Handle")
    .AddColumn("Source Agent Name")
    .AddColumn("Destination Agent Handle")
    .AddColumn("Destination Agent Name")
    .AddColumn("Duration(ms)", true, true)
    .AddColumn("Transfer Size", true, true)
    .AddColumn("Transfer Rate(MB/s)")
    .AddColumn("Thread ID")
    .AddColumn("Call Index");

    for (multiset<HSAMemoryTransferAPIInfo*, HSAMemDurationCmp>::reverse_iterator it = m_memAPISet.rbegin(); it != m_memAPISet.rend(); it++)
    {
        if (count > m_uiTopX)
        {
            break;
        }

        HSAMemoryTransferAPIInfo* info = *it;

        ULONGLONG ullDuration = info->m_transferEndTime - info->m_transferStartTime;
        string strRate;
        string strSize = StringUtils::InsertLeadingSpace(StringUtils::GetDataSizeStr(info->m_size, 2), 15);

        if (ullDuration == 0)
        {
            // Runtime return incorrect timing for Image type object
            // Show NA for zero copy as well.
            strRate = "NA";
        }
        else
        {
            unsigned int mb = 1 << 20;
            double dSize = static_cast<double>(info->m_size) / mb;
            double dRate = dSize / (static_cast<double>(ullDuration) * 1e-9);
            strRate = StringUtils::ToStringPrecision(dRate, 3);  // + "MB/s"; //StringUtils::GetDataSizeStr( (unsigned int)ullRate, 3 ) + "/s";
        }

        HTMLTableRow row(&table);

        std::string keyValues = GenerateHTMLKeyValue(gs_THREAD_ID_TAG, info->m_tid);
        keyValues = AppendHTMLKeyValue(keyValues, GenerateHTMLKeyValue(gs_SEQUENCE_ID_TAG, info->m_uiSeqID));
        keyValues = AppendHTMLKeyValue(keyValues, GenerateHTMLKeyValue(gs_VIEW_TAG, gs_VIEW_TIMELINE_DEVICE_TAG));
        std::string hRef = GenerateHref(keyValues, info->m_uiSeqID);

        auto SplitAgentHandleAndName = [](const std::string& agentString, std::string& agentHandle, std::string& agentName)
        {
            // {handle=13816720,name=Intel(R) Core(TM) i7-6700 CPU @ 3.40GHz}
            std::string tempString = std::string(agentString.begin() + 1, agentString.end() - 1);
            size_t agentHandleNameSeparatorPos = tempString.find(",");

            agentHandle = std::string(tempString.begin(), tempString.begin() + agentHandleNameSeparatorPos);
            size_t nameValueseparatorPos = agentHandle.find(ATP_PARAM_VALUE_DELIMITER);
            agentHandle = std::string(agentHandle.begin() + nameValueseparatorPos + 1, agentHandle.end());

            agentName = std::string(tempString.begin() + agentHandleNameSeparatorPos + 1, tempString.end());
            nameValueseparatorPos = agentName.find(ATP_PARAM_VALUE_DELIMITER);
            agentName = std::string(agentName.begin() + nameValueseparatorPos + 1, agentName.end());
        };


        std::string srcAgentHandle;
        std::string srcAgentName;

        std::string destAgentHandle;
        std::string destAgentName;

        std::string agentString = info->m_strSrcAgent;
        SplitAgentHandleAndName(agentString, srcAgentHandle, srcAgentName);

        agentString = info->m_strDstAgent;
        SplitAgentHandleAndName(agentString, destAgentHandle, destAgentName);

        row.AddItem(0, srcAgentHandle)
        .AddItem(1, srcAgentName)
        .AddItem(2, destAgentHandle)
        .AddItem(3, destAgentName)
        .AddItem(4, StringUtils::NanosecToMillisec(ullDuration))
        .AddItem(5, strSize)
        .AddItem(6, strRate)
        .AddItem(7, StringUtils::ToString(info->m_tid))
        .AddItem(8, hRef);
        table.AddRow(row);

        count++;
    }

    table.WriteToStream(sout);
}

bool HSAMemSummarizer::GenerateHTMLPage(const char* szFileName)
{
    bool retVal = false;

    if (!m_memAPISet.empty())
    {
        ofstream fout(szFileName);
        fout <<
             "<!-- saved from url=(0014)about:internet -->\n"      // add this line so that java script is enabled automatically
             "<html>\n"
             "<head>\n"
             "<title>Top " << m_uiTopX << " Memory Operation Summary Page</title>\n"
             "<META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; charset=ISO-8859-1\">\n";

        HTMLTable::WriteTableStyle(fout);
        HTMLTable::WriteSortableTableScript(fout);

        fout << "</head>\n";
        fout << "<body>\n";

        GenerateHTMLTable(fout);

        fout << "\n";

        fout <<
             "</body>"
             "</html>";

        fout.close();
        retVal = true;
    }

    return retVal;
}
