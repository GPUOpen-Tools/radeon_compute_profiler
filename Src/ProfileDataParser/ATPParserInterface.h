//==============================================================================
// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Interface file for the ATP file parsing
//==============================================================================

#ifndef _ATP_PARSER_INTERFACE_H_
#define _ATP_PARSER_INTERFACE_H_

#ifndef ATP_PARSER_INTERFACE
    /// macro for exporting an API function
    #ifdef _WIN32
        #ifdef __cplusplus
            #define ATP_PARSER_INTERFACE extern "C" __declspec( dllimport )
        #else
            #define ATP_PARSER_INTERFACE __declspec( dllimport )
        #endif
    #else //__linux__
        #ifdef __cplusplus
            #define ATP_PARSER_INTERFACE extern "C"
        #else
            #define ATP_PARSER_INTERFACE extern
        #endif
    #endif
#endif

#ifdef _WIN32
#include <windows.h>
typedef GUID RCP_PROFILE_DATA_PARSER_UUID;
#else
typedef struct _RCP_PROFILE_DATA_PARSER_UUID
{
    unsigned long data1;
    unsigned short data2;
    unsigned short data3;
    unsigned char data4[8];

    bool operator==(_RCP_PROFILE_DATA_PARSER_UUID otherUUID)
    {
        bool isEqual = true;
        isEqual &= data1 == otherUUID.data1;
        isEqual &= data2 == otherUUID.data2;
        isEqual &= data3 == otherUUID.data3;
        isEqual &= data4[0] == otherUUID.data4[0];
        isEqual &= data4[1] == otherUUID.data4[1];
        isEqual &= data4[2] == otherUUID.data4[2];
        isEqual &= data4[3] == otherUUID.data4[3];
        return isEqual;
    }
} RCP_PROFILE_DATA_PARSER_UUID;
#endif

// UUID: c21db0c8-450f-4de3-9d4a-452156bc6930
const RCP_PROFILE_DATA_PARSER_UUID RCP_PROFILE_DATA_PARSER_UUID_VAL =
{
    0xc21db0c8,
    0x450f,
    0x4de3,
    { 0x9d, 0x4a, 0x45, 0x21, 0x56, 0xbc, 0x69, 0x30 }
};

#include "IAtpDataHandler.h"

#define PARSE_ATP_FILE_FUNC_NAME "ParseAtpFile"
#define GET_ATP_DATA_HANDLER_FUNC_NAME "GetAtpDataHandlerInterface"
#define GET_PARSER_UUID_FUNC_NAME "GetParserUUID"

/// Parses the atp file and sets the callback function pointers
/// \param[in] pAtpFileName name of the atp file
/// \param[in] pOnCallbackOnParse callback function on OnParse
/// \param[in] pOnSetApiNumCallBack callback function on SetApiNum
/// \param[in] pOnReportParserCallback callback function pointer on atp parser progress
/// \return flag indicating if the parsing was suceesful or not
ATP_PARSER_INTERFACE bool ParseAtpFile(const char* pAtpFileName,
                                       ParserListener_OnParseCallback pOnCallbackOnParse,
                                       ParserListener_SetApiNumCallback pOnSetApiNumCallBack,
                                       ParserProgress_ReportProgressCallback pOnReportParserCallback);


/// Get the pointer to Atp Data Handler interface
/// \param[out] ppAtpDataHandler pointer to the atp data handler interface
ATP_PARSER_INTERFACE void GetAtpDataHandlerInterface(void** ppAtpDataHandler);

/// Get the parser Interface UUID
/// \return UUID of the parser
ATP_PARSER_INTERFACE RCP_PROFILE_DATA_PARSER_UUID GetParserUUID();

#endif // _ATP_PARSER_INTERFACE_H_
