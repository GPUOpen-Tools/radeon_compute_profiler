//==============================================================================
// Copyright (c) 2015 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief This file contains the function to parse the command line using Boost
//==============================================================================

#ifndef _PARSE_CMD_LINE_H_
#define _PARSE_CMD_LINE_H_

#include <vector>
#include "Config.h"

/// \addtogroup sprofile
// @{

static const unsigned int UNSPECIFIED_OCCUPANCY_INDEX = static_cast<unsigned int>(-1);

/// Parse the command line arguments
/// \param[in]  argc      the number of arguments
/// \param[in]  argv      the array of argument strings
/// \param[out] configOut the output config structure
/// \return true if successful, false otherwise
bool ParseCmdLine(int argc, wchar_t* argv[], Config& configOut);
std::pair<std::string, std::string> Parser(const std::string& s);
// @}

#ifdef CL_TRACE_TEST
    bool get_s_bEncounteredPositional();
    void set_s_bEncounteredPositional(bool val);
    std::string get_s_strInjectedApp();
    void set_s_strInjectedApp(std::string val);
    std::string get_s_strInjectedAppArgs();
    void set_s_strInjectedAppArgs(std::string val);
#endif

#endif
