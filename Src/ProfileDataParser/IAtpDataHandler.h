//==============================================================================
// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Atp Data Handler Interface header
//==============================================================================

#ifndef _I_ATP_DATA_HANDLER_H_
#define _I_ATP_DATA_HANDLER_H_

#include "ISymbolFileEntryInfoDataHandler.h"
#include "IPerfMarkerInfoDataHandler.h"
#include "ICLApiInfoDataHandler.h"
#include "IHSAApiInfoDataHandler.h"
#include "IOccupancyFileInfoDataHandler.h"

/// Enum of the type of the information in atp file
enum AtpInfoType
{
    OPENCL_INFO,            ///< opencl info
    HSA_INFO,               ///< hsa info
    PERF_MARKER_ENTRY,      ///< perf marker entry info
    STACK_TRACE_INFO,       ///< stack trace entry info
    SYMBOL_ENTRY            ///< symbol file entry info
};

typedef void(*ParserListener_OnParseCallback)(AtpInfoType apiInfoType, bool& stopParsing);
typedef void(*ParserListener_SetApiNumCallback)(osThreadId threadId, unsigned int apiNum);
typedef void(*ParserProgress_ReportProgressCallback)(const char* pStrProgressMessage, unsigned int uiCurItem, unsigned int uiTotalItems);


/// Atp File Data Handler Interface
class IAtpDataHandler
{
public:

    /// Get the HSA ApiInfo Interface pointer
    /// \return pointer to the HSA ApiInfo data handler
    virtual IHSAAPIInfoDataHandler* GetHSAApiInfoDataHandler() = 0;

    /// Get the CL ApiInfo Interface pointer
    /// \return pointer to the CL ApiInfo data handler
    virtual ICLAPIInfoDataHandler* GetCLApiInfoDataHandler() = 0;

    /// Get the PerfMarker ApiInfo Interface pointer
    /// \return pointer to the PerfMarker ApiInfo data handler
    virtual IPerfMarkerInfoDataHandler* GetPerfMarkerInfoDataHandler() = 0;

    /// Get the Symbol File Entry ApiInfo Interface pointer
    /// \return pointer to the Symbol File Entry ApiInfo data handler
    virtual ISymbolFileEntryInfoDataHandler* GetSymbolFileEntryInfoDataHandler() = 0;

    /// Get the Occupancy File Parser Handler Interface
    /// \param[in] pOccupancyFileName name of the occupancy file
    /// \return pointer to occupancy file parser handler
    virtual IOccupancyFileInfoDataHandler* GetOccupancyFileInfoDataHandler(const char* pOccupancyFileName) = 0;

    /// Destructor
    virtual ~IAtpDataHandler() = default;
};

#endif // _I_ATP_DATA_HANDLER_H_
