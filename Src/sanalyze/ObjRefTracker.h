//==============================================================================
// Copyright (c) 2015 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  This analyzer class tracks api object references and generates warning messages
//==============================================================================

#ifndef _OBJ_REF_TRACKER_H_
#define _OBJ_REF_TRACKER_H_

//------------------------------------------------------------------------------------
/// Object Action
//------------------------------------------------------------------------------------
enum APIObjectAction
{
    API_OBJECT_ACTION_Create,   ///< Object Create... (increase ref count)
    API_OBJECT_ACTION_Retain,   ///< Object Retain... (increase ref count)
    API_OBJECT_ACTION_Release   ///< Object Release...(decrease ref count)
};

//------------------------------------------------------------------------------------
/// Object history
//------------------------------------------------------------------------------------
struct APIObjectHistory
{
    APIObjectAction m_action;      ///< Object Action
    int             m_iCurrentRef; ///< Current reference counter
    APIInfo*        m_pAPIInfoObj; ///< APIInfo object
};

/// Structure for string compare object
struct StringCompareNoCaseForMap
{
    /// Compares two string ignoring cases and returns the result
    /// \param[in] leftString left string
    /// \param[in] rightString right string
    /// \return true if left string is smaller than right string otherwise false
    bool operator()(const std::string& leftString, const std::string& rightString) const
    {
#ifdef _WIN32
        return _strcmpi(leftString.c_str(), rightString.c_str()) < 0;
#else
        return strcasecmp(leftString.c_str(), rightString.c_str()) < 0;
#endif
    }
};

typedef std::list<APIObjectHistory> APIObjHistoryList;
typedef std::map<std::string, APIObjHistoryList*, StringCompareNoCaseForMap> APITraceMap;

#endif // _OBJ_REF_TRACKER_H_
