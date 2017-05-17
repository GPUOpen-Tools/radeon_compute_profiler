//==============================================================================
// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief ATP file parsing interface implementation
//==============================================================================

#ifdef _LINUX
    #define ATP_PARSER_INTERFACE extern "C"
#else
    #define ATP_PARSER_INTERFACE extern "C" __declspec( dllexport )
#endif

#include "ATPParserInterface.h"
#include "AtpDataHandlerImp.h"

ATP_PARSER_INTERFACE bool ParseAtpFile(const char* pAtpFileName,
                                       ParserListener_OnParseCallback pOnCallbackOnParse,
                                       ParserListener_SetApiNumCallback pOnSetApiNumCallBack,
                                       ParserProgress_ReportProgressCallback pOnReportParserCallback)
{
    AtpDataHandler::Instance()->SetOnParseCallback(pOnCallbackOnParse);
    AtpDataHandler::Instance()->SetOnSetApiNumCallback(pOnSetApiNumCallBack);
    AtpDataHandler::Instance()->SetOnReportProgressCallback(pOnReportParserCallback);
    return AtpDataHandler::Instance()->ParseAtpFile(pAtpFileName);
}

ATP_PARSER_INTERFACE void GetAtpDataHandlerInterface(void** ppAtpDataHandler)
{
    *ppAtpDataHandler = AtpDataHandler::Instance();
}

ATP_PARSER_INTERFACE RCP_PROFILE_DATA_PARSER_UUID GetParserUUID()
{
    return RCP_PROFILE_DATA_PARSER_UUID_VAL;
}

