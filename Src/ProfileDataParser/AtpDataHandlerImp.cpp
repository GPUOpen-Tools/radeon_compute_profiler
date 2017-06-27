//==============================================================================
// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Atp Data Info Handler Implemention
//==============================================================================

#include <ProfilerOutputFileDefs.h>
#include "../CLTraceAgent/CLAtpFile.h"
#include "../HSAFdnTrace/HSAAtpFile.h"
#include "AtpDataHandlerImp.h"
#include "OccupancyFileInfoDataHandlerImp.h"

AtpDataHandler::AtpDataHandler() :  m_pCallbackOnParse(nullptr),
    m_pCallbackOnSetApiNum(nullptr),
    m_pCallbackOnReportProgress(nullptr)
{
}

bool AtpDataHandler::ParseAtpFile(std::string filename)
{
    bool success = true;

    Config config;
    AtpFileParser parser;

    // add part parser for OpenCL API Trace and Timestamp sections
    CLAtpFilePart clAtpPart(config);
    clAtpPart.AddProgressMonitor(this);
    parser.AddAtpFilePart(&clAtpPart);

    // add part parser for HSA API Trace and Timestamp sections
    HSAAtpFilePart hsaAtpPart(config);
    hsaAtpPart.AddProgressMonitor(this);
    parser.AddAtpFilePart(&hsaAtpPart);

    // add part parser for cl Stack Trace section
    StackTraceAtpFilePart clStackPart(CL_PART_NAME, config);
    clStackPart.AddProgressMonitor(this);
    parser.AddAtpFilePart(&clStackPart);

    // add part parser for hsa Stack Trace section
    StackTraceAtpFilePart hsaStackPart(HSA_PART_NAME, config);
    hsaStackPart.AddProgressMonitor(this);
    parser.AddAtpFilePart(&hsaStackPart);

    // add part parser for perfmarker section
    PerfMarkerAtpFilePart perfMarkerPart(config);
    perfMarkerPart.AddProgressMonitor(this);
    parser.AddAtpFilePart(&perfMarkerPart);

    clAtpPart.AddListener(this);
    hsaAtpPart.AddListener(this);
    clStackPart.AddListener(this);
    hsaStackPart.AddListener(this);
    perfMarkerPart.AddListener(this);

    success &= parser.LoadFile(filename.c_str());

    if (success)
    {
        success &= parser.Parse();
    }

    return success;
}

void AtpDataHandler::OnParse(CLAPIInfo* pClApiInfo, bool& stopParsing)
{
    m_clApiInfoDataHandlerList.push_back(new(std::nothrow) CLAPIInfoDataHandler(pClApiInfo));

    if (nullptr != m_pCallbackOnParse)
    {
        m_pCallbackOnParse(OPENCL_INFO, stopParsing);
    }
}

void AtpDataHandler::OnParse(HSAAPIInfo* pHsaApiInfo, bool& stopParsing)
{
    m_hsaApiInfoDataHandlerList.push_back(new(std::nothrow) HSAAPIInfoDataHandler(pHsaApiInfo));

    if (nullptr != m_pCallbackOnParse)
    {
        m_pCallbackOnParse(HSA_INFO, stopParsing);
    }
}

void AtpDataHandler::OnParse(PerfMarkerEntry* pSymbolFileEntry, bool& stopParsing)
{
    m_perfMarkerInfoDataHandlerList.push_back(new(std::nothrow) PerfMarkerInfoDataHandler(pSymbolFileEntry));

    if (nullptr != m_pCallbackOnParse)
    {
        m_pCallbackOnParse(PERF_MARKER_ENTRY, stopParsing);
    }
}

void AtpDataHandler::OnParse(SymbolFileEntry* pPerfMarkerEntry, bool& stopParsing)
{
    m_symbolFileEntryInfoDataHandlerList.push_back(new SymbolFileEntryInfoDataHandler(pPerfMarkerEntry));

    if (nullptr != m_pCallbackOnParse)
    {
        m_pCallbackOnParse(SYMBOL_ENTRY, stopParsing);
    }
}

void AtpDataHandler::SetAPINum(osThreadId threadId, unsigned int apiNum)
{
    if (nullptr != m_pCallbackOnSetApiNum)
    {
        m_pCallbackOnSetApiNum(threadId, apiNum);
    }
}

void AtpDataHandler::OnParserProgress(const std::string& strProgressMessage, unsigned int uiCurItem, unsigned int uiTotalItems)
{
    if (nullptr != m_pCallbackOnReportProgress)
    {
        m_pCallbackOnReportProgress(strProgressMessage.c_str(), uiCurItem, uiTotalItems);
    }
}

void AtpDataHandler::SetOnParseCallback(ParserListener_OnParseCallback pParserCallbackFunc)
{
    m_pCallbackOnParse = pParserCallbackFunc;
}

void AtpDataHandler::SetOnSetApiNumCallback(ParserListener_SetApiNumCallback pSetApiNumCallback)
{
    m_pCallbackOnSetApiNum = pSetApiNumCallback;
}

void AtpDataHandler::SetOnReportProgressCallback(ParserProgress_ReportProgressCallback pReportProgressCallback)
{
    m_pCallbackOnReportProgress = pReportProgressCallback;
}

IHSAAPIInfoDataHandler* AtpDataHandler::GetHSAApiInfoDataHandler()
{
    IHSAAPIInfoDataHandler* pRetHandler = nullptr;

    if (!m_hsaApiInfoDataHandlerList.empty())
    {
        pRetHandler = m_hsaApiInfoDataHandlerList.back();
    }

    return pRetHandler;
}

ICLAPIInfoDataHandler* AtpDataHandler::GetCLApiInfoDataHandler()
{
    ICLAPIInfoDataHandler* pRetHandler = nullptr;

    if (!m_clApiInfoDataHandlerList.empty())
    {
        pRetHandler = m_clApiInfoDataHandlerList.back();
    }

    return pRetHandler;
}

IPerfMarkerInfoDataHandler* AtpDataHandler::GetPerfMarkerInfoDataHandler()
{
    IPerfMarkerInfoDataHandler* pRetHandler = nullptr;

    if (!m_perfMarkerInfoDataHandlerList.empty())
    {
        pRetHandler = m_perfMarkerInfoDataHandlerList.back();
    }

    return pRetHandler;
}

ISymbolFileEntryInfoDataHandler* AtpDataHandler::GetSymbolFileEntryInfoDataHandler()
{
    ISymbolFileEntryInfoDataHandler* pRetHandler = nullptr;

    if (!m_symbolFileEntryInfoDataHandlerList.empty())
    {
        pRetHandler = m_symbolFileEntryInfoDataHandlerList.back();
    }

    return pRetHandler;
}

IOccupancyFileInfoDataHandler* AtpDataHandler::GetOccupancyFileInfoDataHandler(const char* pOccupancyFileName)
{
    IOccupancyFileInfoDataHandler* pRetHandler = nullptr;
    std::map<std::string, IOccupancyFileInfoDataHandler*>::iterator mapIter;
    std::string tempOccupancyFileName(pOccupancyFileName);
    mapIter = m_occupancyFileInfoDataHandlers.find(tempOccupancyFileName);

    if (mapIter != m_occupancyFileInfoDataHandlers.end() && nullptr != mapIter->second)
    {
        pRetHandler = mapIter->second;
    }
    else
    {
        pRetHandler = new(std::nothrow) OccupancyFileInfoDataHandler(tempOccupancyFileName);

        if (nullptr != pRetHandler)
        {
            m_occupancyFileInfoDataHandlers.insert(std::pair<OccupancyFile, IOccupancyFileInfoDataHandler*>(pOccupancyFileName, pRetHandler));
        }
    }

    return pRetHandler;
}


AtpDataHandler::~AtpDataHandler()
{
    for (CLApiInfoDataHandlerList::iterator it = m_clApiInfoDataHandlerList.begin(); it != m_clApiInfoDataHandlerList.end(); ++it)
    {
        if (nullptr != (*it))
        {
            delete(*it);
        }
    }

    m_clApiInfoDataHandlerList.clear();

    for (HSAApiInfoDataHandlerList::iterator it = m_hsaApiInfoDataHandlerList.begin(); it != m_hsaApiInfoDataHandlerList.end(); ++it)
    {
        if (nullptr != (*it))
        {
            delete(*it);
        }
    }

    m_hsaApiInfoDataHandlerList.clear();

    for (PerfMarkerInfoDataHandlerList::iterator it = m_perfMarkerInfoDataHandlerList.begin(); it != m_perfMarkerInfoDataHandlerList.end(); ++it)
    {
        if (nullptr != (*it))
        {
            delete(*it);
        }
    }

    m_perfMarkerInfoDataHandlerList.clear();

    for (SymbolFileEntryInfoDataHandlerList::iterator it = m_symbolFileEntryInfoDataHandlerList.begin(); it != m_symbolFileEntryInfoDataHandlerList.end(); ++it)
    {
        if (nullptr != (*it))
        {
            delete(*it);
        }
    }

    m_symbolFileEntryInfoDataHandlerList.clear();

    for (std::map<OccupancyFile, IOccupancyFileInfoDataHandler*>::iterator it = m_occupancyFileInfoDataHandlers.begin(); it != m_occupancyFileInfoDataHandlers.end(); ++it)
    {
        if (nullptr != it->second)
        {
            delete it->second;
        }
    }

    m_occupancyFileInfoDataHandlers.clear();
}
