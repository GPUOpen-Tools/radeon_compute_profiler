//==============================================================================
// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Atp Data Info Handler Implemention header
//==============================================================================

#ifndef _ATP_DATA_HANDLER_IMP_H_
#define _ATP_DATA_HANDLER_IMP_H_

#include <IParserProgressMonitor.h>
#include <TSingleton.h>

#include "IAtpDataHandler.h"
#include "CLApiInfoDataHandlerImp.h"
#include "HSAApiInfoDataHandlerImpl.h"
#include "PerfMarkerInfoDataHandlerImp.h"
#include "SymbolFileEntryInfoDataHandlerImp.h"

typedef std::string OccupancyFile;
typedef std::vector<ICLAPIInfoDataHandler*> CLApiInfoDataHandlerList;
typedef std::vector<IHSAAPIInfoDataHandler*> HSAApiInfoDataHandlerList;
typedef std::vector<IPerfMarkerInfoDataHandler*> PerfMarkerInfoDataHandlerList;
typedef std::vector<ISymbolFileEntryInfoDataHandler*> SymbolFileEntryInfoDataHandlerList;

/// Atp File data Info Handler class
class AtpDataHandler :  public IAtpDataHandler,
    public IParserListener<CLAPIInfo>,
    public IParserListener<HSAAPIInfo>,
    public IParserListener<SymbolFileEntry>,
    public IParserListener<PerfMarkerEntry>,
    public IParserProgressMonitor,
    public TSingleton<AtpDataHandler>
{
    friend class TSingleton<AtpDataHandler>;
public:

    /// Get the HSA ApiInfo Interface pointer
    /// \return pointer to the HSA ApiInfo data handler
    IHSAAPIInfoDataHandler* GetHSAApiInfoDataHandler() override;

    /// Get the CL ApiInfo Interface pointer
    /// \return pointer to the CL ApiInfo data handler
    ICLAPIInfoDataHandler* GetCLApiInfoDataHandler() override;

    /// Get the PerfMarker ApiInfo Interface pointer
    /// \return pointer to the PerfMarker ApiInfo data handler
    IPerfMarkerInfoDataHandler* GetPerfMarkerInfoDataHandler() override;

    /// Get the Symbol File Entry ApiInfo Interface pointer
    /// \return pointer to the Symbol File Entry ApiInfo data handler
    ISymbolFileEntryInfoDataHandler* GetSymbolFileEntryInfoDataHandler() override;

    /// Get the Occupancy File Parser Handler Interface
    /// \param[in] pOccupancyFileName name of the occupancy file
    /// \return pointer to occupancy file parser handler
    IOccupancyFileInfoDataHandler* GetOccupancyFileInfoDataHandler(const char* pOccupancyFileName) override;

    /// Parse the passed atp file
    /// \param[in] filename name of the atp file
    /// \return flag indicating the parsing of the file successful or not
    bool ParseAtpFile(std::string filename);

    /// Override function for CLApiInfo Parser Lsitener
    /// \param[in] pClApiInfo pointer to cl api info
    /// \param[in,out] stopParsing flag indicating to stop parsing or not
    void OnParse(CLAPIInfo* pClApiInfo, bool& stopParsing) override;

    /// Override function for HSAApiInfo Parser Lsitener
    /// \param[in] pHsaApiInfo pointer to cl api info
    /// \param[in,out] stopParsing flag indicating to stop parsing or not
    void OnParse(HSAAPIInfo* pHsaApiInfo, bool& stopParsing) override;

    /// Override function for symbol file entry Parser Lsitener
    /// \param[in] pSymbolFileEntry pointer to cl api info
    /// \param[in,out] stopParsing flag indicating to stop parsing or not
    void OnParse(SymbolFileEntry* pSymbolFileEntry, bool& stopParsing) override;

    /// Override function for PerfMarker entry Parser Lsitener
    /// \param[in] pPerfMarkerEntry pointer to cl api info
    /// \param[in,out] stopParsing flag indicating to stop parsing or not
    void OnParse(PerfMarkerEntry* pPerfMarkerEntry, bool& stopParsing) override;

    /// Override function for SetApiNum Parser Lsitener
    /// \param[in] threadId thread id of the api
    /// \param[in] apiNum index of the api in the atp file
    void SetAPINum(osThreadId threadId, unsigned int apiNum) override;

    /// Override function for parser progress monitor
    /// \param[in] strProgressMessage progress message for the monitor
    /// \param[in] uiCurItem index of the current item
    /// \param[in] uiTotalItems count of the total items
    void OnParserProgress(const std::string& strProgressMessage, unsigned int uiCurItem, unsigned int uiTotalItems) override;

    /// Sets the OnParse CallBack Parser function
    /// \param[in] pParserCallbackFunc callback function pointer
    void SetOnParseCallback(ParserListener_OnParseCallback pParserCallbackFunc);

    /// Sets the SetApiNum callback parser
    /// \param[in] pSetApiNumCallback callback function pointer
    void SetOnSetApiNumCallback(ParserListener_SetApiNumCallback pSetApiNumCallback);

    /// Sets the reort porgress callback function
    /// \param[in] pReportProgressCallback callback function pointer
    void SetOnReportProgressCallback(ParserProgress_ReportProgressCallback pReportProgressCallback);

    /// Destructor
    ~AtpDataHandler();
private:
    /// Constructor
    AtpDataHandler();

    ParserListener_OnParseCallback                                          m_pCallbackOnParse;                         ///< OnParse callback function pointer
    ParserListener_SetApiNumCallback                                        m_pCallbackOnSetApiNum;                     ///< SetApiNum callback function pointer
    ParserProgress_ReportProgressCallback                                   m_pCallbackOnReportProgress;                ///< parser progress report callback function pointer
    CLApiInfoDataHandlerList                                                m_clApiInfoDataHandlerList;                 ///< map of atpfile and its list of clApiInfodata Handler Interface pointer
    HSAApiInfoDataHandlerList                                               m_hsaApiInfoDataHandlerList;                ///< map of atpfile and its list of hsaApiInfodata Handler Interface pointer
    PerfMarkerInfoDataHandlerList                                           m_perfMarkerInfoDataHandlerList;            ///< map of atpfile and its list of perfMarkerInfodata Handler Interface pointer
    SymbolFileEntryInfoDataHandlerList                                      m_symbolFileEntryInfoDataHandlerList;       ///< symbol File Entry Info data Handler Interface pointer
    std::map<OccupancyFile, IOccupancyFileInfoDataHandler*>                 m_occupancyFileInfoDataHandlers;            ///< Map of occupancy file name and its parser interface pointer
};

#endif // _ATP_DATA_HANDLER_IMP_H_
