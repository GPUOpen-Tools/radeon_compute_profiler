// Copyright (c) 2015 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  This file defines APIInfo descendants used when tracing
//==============================================================================

#ifndef _HSA_API_INFO_H_
#define _HSA_API_INFO_H_

// avoid conflict between Qt, which #defines "signals" and hsa_ext_amd.h which uses "signals" as a parameter name for a function
#if defined(signals)
    #pragma push_macro("signals")
    #undef signals
    #define NEED_TO_POP_SIGNALS_MACRO
#endif
#include "../HSAFdnCommon/HSAFunctionDefs.h"
#if defined (NEED_TO_POP_SIGNALS_MACRO)
    #pragma pop_macro("signals")
#endif

#include "../HSAFdnCommon/HSAFunctionDefsUtils.h"

// std
#include <sstream>
#include <string>

// common
#include <AMDTOSWrappers/Include/osOSDefinitions.h>

// profiler common
#include <ProfilerOutputFileDefs.h>
#include <APIInfo.h>
#include <StringUtils.h>

/// Removes the delimiter sign between param name and value if exists
/// \param argListWithParamName list of the arguments
inline void RemoveParamNameFromArgList(std::vector<std::string>& argListWithParamName)
{
    size_t equalSignPos = std::string::npos;

    for (std::vector<std::string>::iterator it = argListWithParamName.begin(); it != argListWithParamName.end(); ++it)
    {
        equalSignPos = (*it).find_first_of(ATP_PARAM_VALUE_DELIMITER);

        // To-Do: Need to remove this once we update gold file
        size_t paramValPosFromDelimiter = (*it)[equalSignPos + 1] == ' ' ? 2 : 1;

        if (equalSignPos != std::string::npos)
        {
            *it = it->substr(equalSignPos + paramValPosFromDelimiter);
        }
    }
}

//------------------------------------------------------------------------------------
/// HSA API info
//------------------------------------------------------------------------------------
class HSAAPIInfo : public APIInfo
{
public:
    /// Constructor
    HSAAPIInfo() : APIInfo(), m_apiID(HSA_API_Type_UNKNOWN), m_bIsAPI(true)
    {
    }

    /// Virtual destructor
    virtual ~HSAAPIInfo() {}

    /// Parse the argument list -- empty impl in base class
    virtual void ParseArgList() {}

    HSA_API_Type m_apiID;  ///< HSA API ID, defined in HSAFdnCommon/HSAFunctionDefs.h
    bool         m_bIsAPI; ///< flag indicating if this item corresponds to an actual API
};

//------------------------------------------------------------------------------------
/// HSAAPIInfo descendant for memory APIs
//------------------------------------------------------------------------------------
class HSAMemoryAPIInfo : public HSAAPIInfo
{
public:
    /// Constructor
    HSAMemoryAPIInfo() : HSAAPIInfo(), m_size(0)
    {
    }

    /// Virtual destructor
    virtual ~HSAMemoryAPIInfo() {}

    /// Parse the argument list
    virtual void ParseArgList() override
    {
        unsigned int sizeArgPos = 0;
        bool doesAPIHaveSizeArg = false;

        size_t sizeToMultiply = 0;
        bool doesAPINeedToMultiplySize = false;

        unsigned int rectangleArgPos = 0;
        bool doesAPIHaveRectangleArg = false;

        if (HSA_API_Type_UNKNOWN == m_apiID)
        {
            m_apiID = HSAFunctionDefsUtils::Instance()->ToHSAAPIType(m_strName);
        }

        if (m_apiID == HSA_API_Type_hsa_memory_allocate || m_apiID == HSA_API_Type_hsa_memory_register ||
            m_apiID == HSA_API_Type_hsa_memory_deregister || m_apiID == HSA_API_Type_hsa_amd_memory_pool_allocate ||
            m_apiID == HSA_API_Type_hsa_amd_memory_lock)
        {
            doesAPIHaveSizeArg = true;
            sizeArgPos = 1;
        }
        else if (m_apiID == HSA_API_Type_hsa_memory_copy)
        {
            doesAPIHaveSizeArg = true;
            sizeArgPos = 2;
        }
        else if (m_apiID == HSA_API_Type_hsa_amd_interop_map_buffer || m_apiID == HSA_API_Type_hsa_amd_memory_async_copy)
        {
            doesAPIHaveSizeArg = true;
            sizeArgPos = 4;
        }
        else if (m_apiID == HSA_API_Type_hsa_amd_memory_async_copy_rect)
        {
            doesAPIHaveRectangleArg = true;
            rectangleArgPos = 4;
        }
        else if (m_apiID == HSA_API_Type_hsa_amd_memory_fill)
        {
            doesAPIHaveSizeArg = true;
            sizeArgPos = 2;
            doesAPINeedToMultiplySize = true;
            sizeToMultiply = sizeof(uint32_t);
        }

        if (doesAPIHaveSizeArg || doesAPIHaveRectangleArg)
        {
            std::vector<std::string> args;
            m_argList = StringUtils::ReplaceHTMLSymbolsToASCIISymbols(m_argList);
            StringUtils::Split(args, m_argList, ATP_TRACE_ENTRY_ARG_SEPARATOR);
            RemoveParamNameFromArgList(args);

            if (doesAPIHaveSizeArg)
            {
                if (sizeArgPos < args.size())
                {
                    std::istringstream ss(args[sizeArgPos]);
                    ss >> m_size;

                    if (doesAPINeedToMultiplySize)
                    {
                        m_size *= sizeToMultiply;
                    }
                }
            }
            else if (doesAPIHaveRectangleArg)
            {
                if (rectangleArgPos < args.size())
                {
                    std::string dimString = args[rectangleArgPos];
                    size_t openBracketPos = dimString.find("{");
                    size_t closeBracketPos = dimString.find("}");

                    if (0 == openBracketPos && dimString.size() - 1 == closeBracketPos)
                    {
                        dimString = dimString.substr(openBracketPos + 1, dimString.size() - 2);
                    }

                    std::vector<std::string> rectDims;
                    StringUtils::Split(rectDims, dimString, ATP_TRACE_STRUCT_ARG_SEPARATOR);

                    if (0 == rectDims.size())
                    {
                        m_size = 0;
                    }
                    else
                    {
                        m_size = 1;

                        for (auto it = rectDims.begin(); it != rectDims.end(); ++it)
                        {
                            size_t dim = 0;
                            std::istringstream ss(*it);
                            ss >> dim;
                            m_size *= dim;
                        }
                    }
                }
            }
        }
    }

    size_t m_size; ///< size of memory operation
};

//------------------------------------------------------------------------------------
/// HSAMemoryAPIInfo descendant for data transfer memory APIs
//------------------------------------------------------------------------------------
class HSAMemoryTransferAPIInfo : public HSAMemoryAPIInfo
{
public:
    /// Constructor
    HSAMemoryTransferAPIInfo() : HSAMemoryAPIInfo(), m_transferStartTime(0ull), m_transferEndTime(0ull)
    {
    }

    /// Virtual destructor
    virtual ~HSAMemoryTransferAPIInfo() {}

    /// Parse the argument list
    virtual void ParseArgList() override
    {
        HSAMemoryAPIInfo::ParseArgList();

        std::vector<std::string> args;
        StringUtils::Split(args, m_argList, ATP_TRACE_ENTRY_ARG_SEPARATOR);
        RemoveParamNameFromArgList(args);

        if (3 < args.size())
        {
            m_strSrcAgent = args[3];
            m_strDstAgent = args[1];
        }
        else
        {
            m_strSrcAgent = "Unknown_Agent";
            m_strDstAgent = "Unknown_Agent";
        }
    }


    uint64_t    m_transferStartTime; ///< timestamp for when the async copy started
    uint64_t    m_transferEndTime;   ///< timestamp for when the async copy ended
    std::string m_strSrcAgent;       ///< Source agent of mem transfer
    std::string m_strDstAgent;       ///< Destination agent of mem transfer
};

//------------------------------------------------------------------------------------
/// HSAMemoryTransferAPIInfo descendant for data transfer memory rect APIs
//------------------------------------------------------------------------------------
class HSAMemoryTransferRectAPIInfo : public HSAMemoryTransferAPIInfo
{
public:
    /// Parse the argument list
    virtual void ParseArgList() override
    {
        HSAMemoryAPIInfo::ParseArgList();

        std::vector<std::string> args;
        StringUtils::Split(args, m_argList, ATP_TRACE_ENTRY_ARG_SEPARATOR);
        RemoveParamNameFromArgList(args);

        if (7 < args.size())
        {
            bool agentIsSrc = args[6] != "hsaHostToDevice";

            if (agentIsSrc)
            {
                m_strSrcAgent = args[5];
                m_strDstAgent = "{handle=Unknown,name=Host}"; // TODO: try to figure out the real Host device
            }
            else
            {
                m_strDstAgent = args[5];
                m_strSrcAgent = "{handle=Unknown,name=Host}";; // TODO: try to figure out the real Host device
            }
        }
        else
        {
            m_strSrcAgent = "Unknown_Agent";
            m_strDstAgent = "Unknown_Agent";
        }
    }
};
//------------------------------------------------------------------------------------
/// HSA Dispatch API info
//------------------------------------------------------------------------------------
class HSADispatchInfo : public HSAAPIInfo
{
public:
    /// Constructor
    HSADispatchInfo() :
        HSAAPIInfo(),
        m_uDim(0)
    {
        m_bIsAPI = false;
        m_apiID = HSA_API_Type_Non_API_Dispatch;
        m_localGroup[0] = m_localGroup[1] = m_localGroup[2] = 0;
        m_globalGroup[0] = m_globalGroup[1] = m_globalGroup[2] = 0;
    }

    std::string  m_strKernelName;      ///< Kernel name
    ULONGLONG    m_uiKernelHandle;     ///< Kernel handle
    std::string  m_strDeviceName;      ///< Device name
    std::string  m_strDeviceHandle;    ///< Device handle
    unsigned int m_queueIndex;         ///< Queue index
    std::string  m_strQueueHandle;     ///< Queue handle

    std::string m_strGroupWorkSize;    ///< Work-group size
    std::string m_strGlobalWorkSize;   ///< Global work size
#define HSA_MAX_DIM 3
    size_t m_localGroup[HSA_MAX_DIM];  ///< local group size
    size_t m_globalGroup[HSA_MAX_DIM]; ///< global group size
    size_t m_uDim;                     ///< group dimention
};

typedef std::map<osThreadId, std::vector<HSAAPIInfo*> > HSAAPIInfoMap;
typedef std::pair<osThreadId, std::vector<HSAAPIInfo*> > HSAAPIInfoMapPair;
typedef std::vector<HSADispatchInfo*> HSADispatchInfoList;

#endif // _HSA_API_INFO_H_
