//==============================================================================
// Copyright (c) 2015 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  This file contains utilities that are shared by HSA Agent libs
//==============================================================================

#include <iostream>

#include <Logger.h>
#include <Defs.h>

#include "HSAAgentUtils.h"
#include "StringUtils.h"

void HSAAgentsContainer::AddAgent(hsa_agent_t agent, char* pAgentName)
{
    if (m_hsaAgentNameInfoMap.find(agent) == m_hsaAgentNameInfoMap.end())
    {
        size_t agentNameLength = strlen(pAgentName);
        char* pAgentNameForTheMap = new(std::nothrow) char[agentNameLength + 1];

        if (nullptr != pAgentNameForTheMap)
        {
            SP_strcpy(pAgentNameForTheMap, agentNameLength, pAgentName);
            m_hsaAgentNameInfoMap.insert(std::pair<hsa_agent_t, char*>(agent, pAgentNameForTheMap));
        }
    }
}

std::string HSAAgentsContainer::GetAgentName(hsa_agent_t agent) const
{
    std::string retString;
    std::unordered_map<hsa_agent_t, char*>::const_iterator it = m_hsaAgentNameInfoMap.find(agent);

    if (it != m_hsaAgentNameInfoMap.end())
    {
        retString = std::string(it->second);
        retString = StringUtils::ReplaceASCIISymbolsToHTMLSymbols(retString);
    }

    return retString;
}

HSAAgentsContainer::~HSAAgentsContainer()
{
    for (std::unordered_map<hsa_agent_t, char*>::iterator it = m_hsaAgentNameInfoMap.begin(); it != m_hsaAgentNameInfoMap.end(); ++it)
    {
        delete[] it->second;
    }

    m_hsaAgentNameInfoMap.clear();
}


void HSAAgentUtils::SplitHSAAgentStringToHandleAndName(
    const std::string hsaAgentString,
    std::string& hsaAgenthandle,
    std::string& hsaAgentname)
{
    // Format {handle=*****,name=*****}
    std::string tempString = hsaAgentString;

    if (hsaAgentString.find('{') != std::string::npos)
    {
        tempString = std::string(hsaAgentString.begin() + 1, hsaAgentString.end() - 1);
    }

    size_t separatorPosition = tempString.find(",");
    std::string handleString = "handle=";
    std::string nameString = "name=";
    hsaAgenthandle = std::string(tempString.begin() + handleString.size(), tempString.begin() + separatorPosition);
    hsaAgentname = std::string(tempString.begin() + separatorPosition + nameString.size(), tempString.end());
}

std::string HSAAgentUtils::GetHSAAgentString(hsa_agent_t hsaAgent, bool allowFormatting, bool surroundWithStruct)
{
    std::string hsaAgentString;

    if (allowFormatting)
    {
        // Format handle=*****,name=*****
        std::stringstream ss;
        ss << "handle=" << StringUtils::ToString(hsaAgent.handle) << ",name=" << HSAAgentsContainer::Instance()->GetAgentName(hsaAgent);
        hsaAgentString = ss.str();
    }
    else
    {
        hsaAgentString = StringUtils::ToString(hsaAgent.handle);
    }

    if (surroundWithStruct)
    {
        hsaAgentString = "{" + hsaAgentString + "}";
    }

    return hsaAgentString;
}

bool CheckRuntimeToolsLibLoaded(uint64_t runtimeVersion, uint64_t failedToolCount, const char* const* pFailedToolNames)
{
    bool retVal = true;

    if (failedToolCount > 0 && runtimeVersion > 0)
    {
        if (nullptr == pFailedToolNames)
        {
            GPULogger::Log(GPULogger::logERROR, "Error loading HSA Profiler. Unknown tool library failed to load\n");
            retVal = false;
        }
        else
        {
            for (uint64_t i = 0; i < failedToolCount && retVal; i++)
            {
                if (nullptr == pFailedToolNames[i])
                {
                    GPULogger::Log(GPULogger::logERROR, "Error loading HSA Profiler. Unknown tool library failed to load\n");
                    retVal = false;
                }
                else
                {
                    std::string failedToolName = std::string(pFailedToolNames[i]);

                    if (std::string::npos != failedToolName.find_last_of(HSA_RUNTIME_TOOLS_LIB))
                    {
                        GPULogger::Log(GPULogger::logERROR, "Error loading HSA Profiler. %s could not be loaded due to version mismatch with the HSA runtime\n", HSA_RUNTIME_TOOLS_LIB);
                        retVal = false;
                    }
                }
            }
        }
    }

    return retVal;
}
