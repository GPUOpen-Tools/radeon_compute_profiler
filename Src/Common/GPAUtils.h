//==============================================================================
// Copyright (c) 2015-2018 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief GPUPerfAPI Utilities
//==============================================================================

#ifndef _GPA_UTILS_H_
#define _GPA_UTILS_H_

#include <map>
#include <vector>
#include <string>

#ifdef AMDT_INTERNAL
    #define USE_INTERNAL_GPA // this define tells GPAInterfaceLoader.h that this is an internal build
#endif
#include "GPAInterfaceLoader.h"

#include "GPUPerfAPICounterLoader.h"
#include "DeviceInfo.h"

#include <AMDTBaseTools/Include/gtString.h>

typedef std::vector<std::string> CounterList;
typedef std::map<GPA_Hw_Generation, CounterList> HWCounterMap;
typedef std::map<gpa_uint32, CounterList> HWCounterDeviceMap;

#define GPA_INFINITE_PASS 0xFFFFFFFF

/// \addtogroup Common
// @{

typedef void (*CounterNameMappingProc)(CounterList& counters);

/// This class handles the interaction with the GPUPerfAPI library.
class GPAUtils
{
public:

    /// Constructor
    GPAUtils();

    /// Destructor
    ~GPAUtils() {}

    /// Accessor to the GPA function table
    /// \return the GPA function table
    GPAFunctionTable* GetGPAFuncTable()
    {
        return m_gpaFuncTable;
    }

    /// Initialize GPA with a context (command queue or ID3D11Device)
    /// \param context Context object
    /// \return true if successful
    bool Open(void* context);


    /// Close the current context in GPA
    /// \return true if successful
    bool Close();

    /// Accessor to whether or not GPA has been loaded
    /// \return true if GPA dll has been loaded; false otherwise
    bool Loaded()
    {
        return m_gpaLoaded;
    }

    /// Get available counters from GPA offline
    /// \param generation Hardware generation
    /// \param availableCounters Output list of counters (optionally with descriptions)
    /// \param shouldIncludeCounterDescriptions optional param to add description to the output list of the counters
    /// \return true if successful
    bool GetAvailableCounters(GPA_Hw_Generation generation, CounterList& availableCounters, const bool shouldIncludeCounterDescriptions = false);

    /// Get available counters from GPA offline
    /// \param generation Hardware generation
    /// \param availableCounters Output list of counters
    /// \return true if successful
    bool GetAvailableCountersGdt(GDT_HW_GENERATION generation, CounterList& availableCounters);

    /// Get available counters from GPA offline
    /// \param uDeviceId Device Id
    /// \param uRevisionId Revision Id
    /// \param nMaxPass Maximum number of passes allowed
    /// \param availableCounters Output list of counters
    /// \return true if successful
    bool GetAvailableCountersForDevice(gpa_uint32 uDeviceId, gpa_uint32 uRevisionId, size_t nMaxPass, CounterList& availableCounters);

    /// Get available counters from GPA offline
    /// \param uDeviceId Device Id
    /// \param uRevisionId Revision Id
    /// \param availableCounters Output list of counters
    /// \param shouldIncludeCounterDescriptions optional param to add description to the output list of the counters
    /// \return true if successful
    bool GetAvailableCountersForDeviceWithoutMaxPass(gpa_uint32 uDeviceId, gpa_uint32 uRevisionId, CounterList& availableCounters, const bool shouldIncludeCounterDescriptions = false);

    /// Sets the list of enabled counters. Can be called after InitGPA to modify the set of enabled counters
    /// \param countersToEnable list of counters to enable
    /// \return true if successful
    bool SetEnabledCounters(const CounterList& countersToEnable);

    /// If SetEnabledCounters is called, selected counters will be enabled, if not, all counters are enabled
    /// \param sessionId GPA session Id
    /// \return true if successful, false otherwise
    bool EnableCounters(GPA_SessionId sessionId);

    /// Load GPA Dll, load counter files if specified
    /// \param api the API to initialize
    /// \param strDLLPath GPA DLL Path
    /// \param strError Error string output
    /// \param pszCounterFile Counter file path
    /// \param[out] pSelectedCounters the list of selected counters
    /// \param nMaxPass Maximum number of passes allowed
    /// \return true if GPAUtils has been initialized.
    bool InitGPA(GPA_API_Type api,
                 const gtString& strDLLPath,
                 std::string& strError,
                 const char* pszCounterFile = NULL,
                 CounterList* pSelectedCounters = NULL,
                 size_t nMaxPass = GPA_INFINITE_PASS);

    /// unloads the currently loaded GPA dll
    void Unload()
    {
        GPAApiManager::Instance()->UnloadApi(m_api);
        m_HWCounterMap.clear();
        m_bInit = false;
    }

    /// Is GPAUtils initialized
    /// \return true if GPAUtils is initialized
    bool IsInitialized() const
    {
        return m_bInit;
    }

    /// Check GPA status code
    /// \param status GPA status code
    /// \return the passed-in status code
    GPA_Status StatusCheck(GPA_Status status);

    /// Get counter value
    /// \param[in] sessionId GPA Session Id
    /// \param[out] counterDataTable counter result array
    /// \param[out] sampleCount Output sample count
    /// \param[out] count Output Number of enabled counters
    void GetCounterValues(GPA_SessionId sessionId, CounterList& counterDataTable, gpa_uint32& sampleCount, gpa_uint32& count);

    /// Verify counter name - This is used to remove unknown counter name specified in the counter file
    /// \param strCounter Counter name
    /// \param generation Hardware generation
    void VerifyCounter(const std::string& strCounter, GPA_Hw_Generation& generation);

    /// Get the counter data type of the specified counter
    /// \param[in] counterIndex the index of the counter whose data type is needed
    /// \param[out] counterDataType the data type of the specified counter
    /// \return true if the counter data type was sucessfully retrieved, false otherwise
    bool GetCounterDataType(gpa_uint32 counterIndex, GPA_Data_Type& counterDataType) const;

    /// Get the counter name of the specified counter
    /// \param[in] counterIndex the index of the counter whose name is needed
    /// \param[out] counterName the name of the specified counter
    /// \return true if the counter name was sucessfully retrieved, false otherwise
    bool GetCounterName(gpa_uint32 counterIndex, std::string& counterName) const;

    /// Creates a GPA Session
    /// \param[out] the session id of the new session
    /// \return true if the session was sucessfully created, false otherwise
    bool CreateSession(GPA_SessionId& sessionId) const;

private:
    /// Disable copy constructor
    GPAUtils(const GPAUtils& gpautils);

    /// Disable assignment operator
    GPAUtils& operator= (const GPAUtils& gpautils);

    /// Get counter by generation, non-thread safe
    /// \param generation Hardware generation
    /// \param shouldIncludeCounterDescriptions optional param to add description to the output list of the counters
    /// \return the list of counters for thie specified generation
    CounterList& GetCounters(GPA_Hw_Generation generation, const bool shouldIncludeCounterDescriptions = false);

    /// Get counters by deviceID, non-thread safe
    /// \param uDeviceid the device id to get counters for
    /// \param uRevisionid the revision id to get counters for
    /// \param nMaxPass Maximum number of passes allowed
    /// \return the list of counters for thie specified device
    CounterList& GetCountersForDevice(gpa_uint32 uDeviceid, gpa_uint32 uRevisionid, size_t nMaxPass);

    /// Get counters by deviceID, non-thread safe
    /// \param uDeviceid the device id to get counters for
    /// \param uRevisionid the revision id to get counters for
    /// \param shouldIncludeCounterDescriptions optional param to add description to the output list of the counters
    /// \return the list of counters for thie specified device
    CounterList& GetCountersForDevice(gpa_uint32 uDeviceid, gpa_uint32 uRevisionid, const bool shouldIncludeCounterDescriptions);

    /// Helper function to enable list of counters
    /// \param sessionId GPA session Id
    /// \param selectedCounterNames Selected counter names
    /// \return true if successful
    bool EnableCounterSet(GPA_SessionId sessionId, const CounterList& selectedCounterNames);

    /// Convert GDT HW genration enum to GPA HW generation enum
    /// \param[in] gdtHwGen The GDT HW generation value
    /// \return GPA Hw generation that matches the input GDT HW generation
    GPA_Hw_Generation GdtHwGenToGpaHwGen(const GDT_HW_GENERATION gdtHwGen);

    /// Logging callback function for GPA
    /// \param messageType the type of message being logged
    /// \param message the message to log
    static void GPALogCallback(GPA_Logging_Type messageType, const char* message);

private:
    GPAFunctionTable*      m_gpaFuncTable;                   ///< the loader that is used to load GPA dlls
    unsigned int           m_nMaxNumCounter;                 ///< Maximum number of counters selected
    CounterList            m_selectedCounters;               ///< List of selected counters - NOTE: If counter file not specified, list is empty and all counter will be enabled.
    GPA_GetAvailableCountersByGenerationProc m_pGetAvailableCountersByGen;     ///< Function to retrieve list of counters for a particular hw generation
    GPA_GetAvailableCountersForDeviceProc    m_pGetAvailableCountersForDevice; ///< Function to retrieve list of counters for a particular device
    HWCounterMap           m_HWCounterMap;                   ///< Hardware generation to counter list map
    HWCounterDeviceMap     m_HWCounterDeviceMap;             ///< Device Id to counter list map
    GPA_API_Type           m_api;                            ///< API type
    bool                   m_bInit;                          ///< A flag indicating whether or not GPAUtils is initialized
    size_t                 m_nMaxPass;                       ///< Max pass allowed
    GPA_ContextId          m_contextId;                      ///< GPA Context
    bool                   m_gpaLoaded;                      ///< Flag indicating whether or not GPA is loaded
};

// @}

#endif //_GPA_UTILS_H_
