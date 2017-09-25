//==============================================================================
// Copyright (c) 2015 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  This file contains utilities that are shared by HSA Agent libs
//==============================================================================
#ifndef _HSA_AGENT_UTILS_H_
#define _HSA_AGENT_UTILS_H_

#include <hsa.h>
#include <TSingleton/TSingleton.h>
#include <unordered_map>
#include <functional>

namespace std
{
/// Specialization struct for hash of type hsa_agent
template<>
struct hash<hsa_agent_t>
{
    /// cast operator overloading
    /// \param[in] agent hsa agent
    /// \return hsa agent
    size_t operator()(const hsa_agent_t agent) const
    {
        return static_cast<size_t>(agent.handle);
    }
};

/// Specialization of equal to for hsa_agent
template<>
struct equal_to<hsa_agent_t>
{
    /// cast operator overloading
    /// \param[in] left hsa agent
    /// \param[in] right hsa agent
    /// \return true is both agents are same otherwise false
    constexpr bool operator()(const hsa_agent_t& left, const hsa_agent_t& right) const
    {
        return left.handle == right.handle;
    }
};
}

class HSAAgentsContainer : public TSingleton<HSAAgentsContainer>
{
    friend class TSingleton<HSAAgentsContainer>;

public:

    /// Adds the hsa agent to the list
    /// \param[in] agent hsa agent
    /// \param[in] pAgentName agent name
    void AddAgent(hsa_agent_t agent, char* pAgentName);

    /// Returns the agent name for agent handle
    /// \param[in] agent agent handle
    /// \return name of the hsa agent
    std::string GetAgentName(hsa_agent_t agent) const;

    /// Destructor
    ~HSAAgentsContainer();

private:

    std::unordered_map<hsa_agent_t, char*>    m_hsaAgentNameInfoMap;    ///< unordered map container for agent handle - agent name
};

namespace HSAAgentUtils
{
/// Splits the formatted hsa agent string
/// \param[in] hsaAgentString formatted agent string
/// \param[out] hsaAgenthandle hsa agent handle string
/// \param[out] hsaAgentname hsa agent name string
void SplitHSAAgentStringToHandleAndName(
    const std::string hsaAgentString,
    std::string& hsaAgenthandle,
    std::string& hsaAgentname);

/// Returns the formatted hsa agent string
/// \param[in] hsaAgent hsa agent
/// \param[in,opt] allowFormatting flag indicating to apply the format or not
/// \param[in,opt] surroundWithStruct flag indicating to add surrounding braces or not
/// \return hsa agent string
std::string GetHSAAgentString(
    hsa_agent_t hsaAgent,
    bool allowFormatting = false,
    bool surroundWithStruct = false);
}

/// Checks whether the hsa runtime tools lib failed to load (most likely due to version check failure)
/// \param runtimeVersion   the runtime version number (passed to an agent's OnLoad function)
/// \param failedToolCount  the number of tools which failed to load (passed to an agent's OnLoad function)
/// \param pFailedToolNames the list of tools which failed to load (passed to an agent's OnLoad function)
/// \return false if the runtime tools lib failed to load (or if an error condition is detected)
bool CheckRuntimeToolsLibLoaded(uint64_t runtimeVersion, uint64_t failedToolCount, const char* const* pFailedToolNames);

#endif // _HSA_AGENT_UTILS_H_
