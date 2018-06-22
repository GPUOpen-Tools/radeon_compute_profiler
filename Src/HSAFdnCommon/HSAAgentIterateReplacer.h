//==============================================================================
// Copyright (c) 2016 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  This file contains a class to replace a user-specified agent
///         iterator callback
//==============================================================================

#ifndef _HSA_AGENT_ITERATE_REPLACER_H
#define _HSA_AGENT_ITERATE_REPLACER_H

#include <unordered_map>

#include "hsa.h"
#include "hsa_api_trace.h"

#include <TSingleton.h>

/// Singleton class to replace a user-specified agent iterator
class HSAAgentIterateReplacer : public TSingleton<HSAAgentIterateReplacer>
{
public:
    /// typedef for an agent iterator callback
    typedef hsa_status_t (*HSAAgentIteratorCallback)(hsa_agent_t agent, void* data);

    /// Gets the agent iterator function to pass to hsa_iterate_agents
    /// \param userCallback the callback function supplied by the user application
    /// \return the callback to actually pass to hsa_iterate_agents
    HSAAgentIteratorCallback GetAgentIterator(HSAAgentIteratorCallback userCallback, CoreApiTable* pRealCoreFunctions);

    /// The profiler's supplied agent iterator callback
    static hsa_status_t ReplacedIterateAgentsCallback(hsa_agent_t agent, void* data);

    /// Gets the GPU Index of the specified agent handle
    /// \param agentHandle the agent handle for the agent whose GPU Index is needed
    /// \param[out] gpuIndex the gpu index of the specified agent handle
    /// \return true if the gpu index of the specified agent handle is known, false otherwise
    static bool GetAgentGPUIndex(uint64_t agentHandle, unsigned int& gpuIndex);

private:
    static HSAAgentIteratorCallback                   m_userSepcifiedIterateAgentsCallback; ///< the user-supplied iterator callback
    static unsigned int                               m_gpuAgentCount;                      ///< the number of agents
    static CoreApiTable*                              m_pRealCoreFunctions;                 ///< the real core function pointer table
    static std::unordered_map<uint64_t, unsigned int> m_agentHandleToGPUIndexMap;           ///< map of agent handles to profiler-defined gpu index
};

#endif // _HSA_AGENT_ITERATE_REPLACER_H

