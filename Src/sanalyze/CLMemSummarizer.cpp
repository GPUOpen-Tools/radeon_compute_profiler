//==============================================================================
// Copyright (c) 2015 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  This file provides the CL Memory Summarizer
//==============================================================================

#include <fstream>
#include <sstream>
#include "CLMemSummarizer.h"
#include "AnalyzerHTMLUtils.h"
#include "../Common/HTMLTable.h"
#include "../Common/StringUtils.h"

CLMemSummarizer::CLMemSummarizer(void)
{
    m_uiTopX = 10;
}

CLMemSummarizer::~CLMemSummarizer(void)
{
}

void CLMemSummarizer::OnParse(CLAPIInfo* pAPIInfo, bool& stopParsing)
{
    stopParsing = false;

    if (pAPIInfo->m_Type == CL_ENQUEUE_MEM)
    {
        CLMemAPIInfo* pMAPI = (CLMemAPIInfo*)pAPIInfo;

        if (!pMAPI->m_bInfoMissing)
        {
            //CL_FUNC_TYPE_clEnqueueMapBuffer = 63
            //CL_FUNC_TYPE_clEnqueueMapImage = 64
            //CL_FUNC_TYPE_clEnqueueUnmapMemObject = 65
            if (pMAPI->m_ullComplete < pMAPI->m_ullRunning && (pMAPI->m_uiAPIID < CL_FUNC_TYPE_clEnqueueMapBuffer || pMAPI->m_uiAPIID > CL_FUNC_TYPE_clEnqueueUnmapMemObject))
            {
                // Complete and Running can be the same for Zero copy.
                return;
            }

            m_memAPISet.insert(pMAPI);
        }
    }
}

void CLMemSummarizer::Debug()
{
}

/// Generate HTML table from statistic data and write to std::ostream
/// \param sout output stream
void CLMemSummarizer::GenerateTopXHTMLTable(std::ostream& sout, bool useTopX)
{
    HTMLTable table;

    unsigned int count = 0;

    table.AddColumn("Command Type")
        .AddColumn("Context ID")
        .AddColumn("Command Queue ID")
        .AddColumn("Duration(ms)", true, true)
        .AddColumn("Transfer Size", true, true)
        .AddColumn("Transfer Rate(MB/s)", true, true)
        .AddColumn("Thread ID")
        .AddColumn("Call Index");

    for (std::multiset<CLMemAPIInfo*, CLMemDurationCmp>::reverse_iterator it = m_memAPISet.rbegin(); it != m_memAPISet.rend(); it++)
    {
        if (useTopX && count > m_uiTopX)
        {
            break;
        }

        CLMemAPIInfo* info = *it;

        ULONGLONG ullDuration = info->m_ullComplete - info->m_ullRunning;
        std::string strRate;
        std::string strSize = StringUtils::InsertLeadingSpace(StringUtils::GetDataSizeStr(info->m_uiTransferSize, 2), 15);

        if (ullDuration == 0 || ((info->m_strCMDType.find("IMAGE") != std::string::npos || info->m_strCMDType.find("MAP") != std::string::npos) && ullDuration < 1000))
        {
            // Runtime return incorrect timing for Image type object
            // Show NA for zero copy as well.
            strRate = "NA";
        }
        else
        {
            unsigned int mb = 1 << 20;
            double dSize = (double)info->m_uiTransferSize / mb;
            double dRate = dSize / ((double)ullDuration * 1e-9);
            strRate = StringUtils::ToStringPrecision(dRate, 3);  // + "MB/s"; //StringUtils::GetDataSizeStr( (unsigned int)ullRate, 3 ) + "/s";
        }

        HTMLTableRow row(&table);

        std::string keyValues;
        keyValues = GenerateHTMLKeyValue(gs_THREAD_ID_TAG, info->m_tid);
        keyValues = AppendHTMLKeyValue(keyValues, GenerateHTMLKeyValue(gs_SEQUENCE_ID_TAG, info->m_uiSeqID));
        keyValues = AppendHTMLKeyValue(keyValues, GenerateHTMLKeyValue(gs_VIEW_TAG, gs_VIEW_TIMELINE_DEVICE_TAG));
        std::string hRef = GenerateHref(keyValues, info->m_strCMDType.substr(11));

        row.AddItem(0, hRef)
        .AddItem(1, StringUtils::ToString(info->m_uiContextID))
        .AddItem(2, StringUtils::ToString(info->m_uiQueueID))
        .AddItem(3, StringUtils::NanosecToMillisec(ullDuration))
        .AddItem(4, strSize)
        .AddItem(5, strRate)
        .AddItem(6, StringUtils::ToString(info->m_tid))
        .AddItem(7, info->m_bHasDisplayableSeqId ? StringUtils::ToString(info->m_uiDisplaySeqID) : "N/A");
        table.AddRow(row);

        count++;
    }

    table.WriteToStream(sout);
}

void CLMemSummarizer::GenerateDataTransferSummaryHTMLTable(std::ostream& sout)
{
    HTMLTable table;

    unsigned int count = 0;

    table.AddColumn("Command Type")
        .AddColumn("Context ID")
        .AddColumn("Command Queue ID")
        .AddColumn("Number of Transfers")
        .AddColumn("Total Duration(ms)", true, true)
        .AddColumn("Total Transfer Size", true, true)
        .AddColumn("Average Transfer Rate(MB/s)");

    typedef std::pair<std::string, std::string> DataTransferPair;

    struct DataTransferSummaryInfo
    {
        std::string m_cmdType;
        std::string m_queueId;
        std::string m_contextId;
        unsigned int m_transferCount = 0;
        uint64_t m_totalTime = 0;
        size_t m_totalSize = 0;
    };

    std::map<DataTransferPair, DataTransferSummaryInfo> dataTransferSet;

    for (std::multiset<CLMemAPIInfo*, CLMemDurationCmp>::reverse_iterator it = m_memAPISet.rbegin(); it != m_memAPISet.rend(); ++it)
    {
        CLMemAPIInfo* info = *it;

        std::string cmdType = info->m_strCMDType;
        std::string queueId = StringUtils::ToString(info->m_uiQueueID);
        uint64_t duration = info->m_ullComplete - info->m_ullRunning;

        DataTransferPair pair = { cmdType.substr(11), queueId };

        DataTransferSummaryInfo summaryInfo;

        if (0 < dataTransferSet.count(pair))
        {
            summaryInfo = dataTransferSet[pair];
            summaryInfo.m_totalSize += info->m_uiTransferSize;
            summaryInfo.m_totalTime += duration;
            summaryInfo.m_transferCount++;
        }
        else
        {
            summaryInfo.m_cmdType = cmdType;
            summaryInfo.m_contextId = StringUtils::ToString(info->m_uiContextID);
            summaryInfo.m_queueId = queueId;
            summaryInfo.m_totalSize += info->m_uiTransferSize;
            summaryInfo.m_totalTime += duration;
            summaryInfo.m_transferCount++;
        }

        dataTransferSet[pair] = summaryInfo;
    }

    for (std::map<DataTransferPair, DataTransferSummaryInfo>::iterator it = dataTransferSet.begin(); it != dataTransferSet.end(); ++it)
    {
        HTMLTableRow row(&table);

        std::string strRate;

        if (it->second.m_totalSize == 0 || ((it->second.m_cmdType.find("IMAGE") != std::string::npos || it->second.m_cmdType.find("MAP") != std::string::npos) && it->second.m_totalTime < 1000))
        {
            // Runtime return incorrect timing for Image type object
            // Show NA for zero copy as well.
            strRate = "NA";
        }
        else
        {
            unsigned int mb = 1 << 20;
            double dSize = static_cast<double>(it->second.m_totalSize) / mb;
            double dRate = dSize / (static_cast<double>(it->second.m_totalTime) * 1e-9);
            strRate = StringUtils::ToStringPrecision(dRate, 3);  // + "MB/s"; //StringUtils::GetDataSizeStr( (unsigned int)ullRate, 3 ) + "/s";
        }

        row.AddItem(0, it->first.first)
            .AddItem(1, it->second.m_contextId)
            .AddItem(2, it->first.second)
            .AddItem(3, StringUtils::ToString(it->second.m_transferCount))
            .AddItem(4, StringUtils::NanosecToMillisec(it->second.m_totalTime))
            .AddItem(5, StringUtils::InsertLeadingSpace(StringUtils::GetDataSizeStr(it->second.m_totalSize, 2), 15))
            .AddItem(6, strRate);
        table.AddRow(row);

        count++;
    }

    table.WriteToStream(sout);
}

bool CLMemSummarizer::GenerateDataTransferHTMLPage(const char* szFileName)
{
    bool retVal = false;

    if (!m_memAPISet.empty())
    {
        std::ofstream fout(szFileName);
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

        GenerateDataTransferSummaryHTMLTable(fout);

        fout << "\n";

        fout <<
             "</body>"
             "</html>";

        fout.close();
        retVal = true;
    }

    return retVal;
}

bool CLMemSummarizer::GenerateTopXDataTransferHTMLPage(const char* szFileName, bool useTopX)
{
    bool retVal = false;

    if (!m_memAPISet.empty())
    {
        std::ofstream fout(szFileName);
        fout <<
             "<!-- saved from url=(0014)about:internet -->\n"      // add this line so that java script is enabled automatically
             "<html>\n"
             "<head>\n";

        if (useTopX)
        {
            fout << "<title>Top " << m_uiTopX << " Memory Operation Summary Page</title>\n";
        }
        else
        {
            fout << "<title>Memory Operation List Page</title>\n";
        }

        fout << "<META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; charset=ISO-8859-1\">\n";

        HTMLTable::WriteTableStyle(fout);
        HTMLTable::WriteSortableTableScript(fout);

        fout << "</head>\n";
        fout << "<body>\n";

        GenerateTopXHTMLTable(fout, useTopX);

        fout << "\n";

        fout <<
             "</body>"
             "</html>";

        fout.close();
        retVal = true;
    }

    return retVal;
}