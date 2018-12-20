//==============================================================================
// Copyright (c) 2015-2018 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  This class interfacts with GPA to retrieve PMC and write the output file
//==============================================================================

#ifndef _HSA_GPA_PROFILE_H_
#define _HSA_GPA_PROFILE_H_

#include <string>
#include <unordered_map>
#include <set>
#include <mutex>
#include <hsa.h>
#include "../Common/GPAUtils.h"
#include "../Common/GlobalSettings.h"
#include "TSingleton.h"
#include "../Common/KernelStats.h"
#include "../Common/ProfilerTimer.h"
#include "GPUPerfAPI-ROCm.h"

#if defined (_LINUX) || defined (LINUX)
#include "HSAKernelAssembly.h"
#endif

/// Handle the response on the end of the timer
/// \param timerType type of the ending timer for which response have to be executed
void HSAGPAProfilerTimerEndResponse(ProfilerTimerType timerType);

//------------------------------------------------------------------------------------
/// This class interfaces with GPA to retrieve PMC and save to file
//------------------------------------------------------------------------------------
class HSAGPAProfiler : public TSingleton<HSAGPAProfiler>
{
    friend class TSingleton<HSAGPAProfiler>;
public:
    /// Initialize GPA, load counter file
    /// \param params Parameters set by rcprof
    /// \param strErrorOut Error string
    /// \return true if succeeded
    bool Init(const Parameters& params, std::string& strErrorOut);

    // Thread safe - concurrent sessions are serialized
    /// \param pRocProfilerData the ROCProfiler callback data
    /// \return true if succeeded
    bool Begin(const rocprofiler_callback_data_t* pRocProfilerData);

    /// End PMC session, generate profiling result
    /// \return true if succeeded
    bool End();

    /// This function waits for a session for the specified queue and and writes out results if one is found
    /// \param queueId the ID of the queue whose sessions should be checked
    /// \param timeoutSeconds the number of seconds to wait for the session before timing out
    /// \return true if the session is complete, false if there is no session for the specified queue or if the session did not complete in the allotted time
    bool WaitForCompletedSession(uint64_t queueId, uint32_t timeoutSeconds = m_DEFAULT_MAX_SECONDS_TO_WAIT_FOR_SESSIONS);

    /// This function waits for all session for all queues and and writes out results if any are found
    /// \param timeoutSeconds the number of seconds to wait for any session before timing out
    void WaitForCompletedSessions(uint32_t timeoutSeconds = m_DEFAULT_MAX_SECONDS_TO_WAIT_FOR_SESSIONS);

    /// Determines whether the max number of kernels have been profiled already
    /// \return true if the max number of kernels have already been profiled
    bool HasKernelMaxBeenReached() const { return m_uiCurKernelCount >= m_uiMaxKernelCount; }

    /// Indicates whether or not profiling is currently enabled
    /// \return true if profiling is enabled, false otherwise
    bool IsProfilingEnabled() const { return m_isProfilingEnabled; }

    /// Enable to disable profiling
    /// \param doEnable, flag indicating whether to enable (true) or disable (false) profiling
    void EnableProfiling(bool doEnable) { m_isProfilingEnabled = doEnable; }

    /// Indicates whether profiler should run after delay or not
    /// \param delayInMilliseconds to return the amount by which profile set to be delayed
    /// \returns true if delay is enabled otherwise false
    bool IsProfilerDelayEnabled(unsigned long& delayInMilliseconds);

    /// Indicates whether profiler should run only for set duration or not
    /// \param durationInMilliseconds to return the amount by which profile set to run
    /// \returns true if duration of the profiler is enabled
    bool IsProfilerDurationEnabled(unsigned long& durationInMilliseconds);

    /// Assigns the call back function
    /// \param timerType type of the timer
    /// \param timerEndHandler call back function pointer
    void SetTimerFinishHandler(ProfilerTimerType timerType, TimerEndHandler timerEndHandler);

    /// Creates the Profiler Timer
    /// \param timerType timer type of the starting timer
    /// \param timeIntervalInMilliseconds profiler duration or profiler delay in milliseconds
    void CreateTimer(ProfilerTimerType timerType, unsigned int timeIntervalInMilliseconds);

    /// Starts the timer
    /// \param timerType timer type of the starting timer
    void StartTimer(ProfilerTimerType timerType);

protected:
    /// Constructor
    HSAGPAProfiler(void);

    /// Destructor
    ~HSAGPAProfiler(void);

private:

    /// Helper function used to determine if the specified agent is a GPU device
    /// \param agent the agent to check
    /// \return true if the specified agent is a GPU device, false otherwise
    static bool IsGPUDevice(hsa_agent_t agent);

    /// Callback used to get all HSA-capable GPU devices on the system
    /// \param agent the agent to check
    /// \param pData the user-defined data (the vector of GPU device IDs)
    /// \return status code indicating whether to continue iterating (HSA_STATUS_SUCCESS) or not (any other value)
    static hsa_status_t GetGPUDeviceIDs(hsa_agent_t agent, void* pData);

    // Set output file
    /// \param strOutputFile Output file
    void SetOutputFile(const std::string& strOutputFile);

    /// Init header
    void InitHeader();

    /// Struct used in the session map
    typedef struct
    {
        GPA_SessionId  m_sessionID;    ///< the GPA session ID for a given session
        KernelStats    m_kernelStats;  ///< the Kernel Statistics for a given session
        std::string    m_agentName;    ///< the name of the agent (device)
        bool           m_sessionEnded; ///< flag indicating if the session has been ended
    } SessionInfo;

    /// Writes the results from the specified session to the session output file
    /// \param sessionId the session whose results should be written
    /// \param kernelStats the kernel statistics to write with the session
    /// \return true is results are written successfully, false otherwise
    bool WriteSessionResult(const SessionInfo& sessionInfo);

    /// This function checks for a completed session for the specified queue and and writes out results if one is found
    /// \param queueId the ID of the queue whose sessions should be checked
    /// \return true if the session is complete, false if there is no session for the specified queue or if the session is not complete
    bool CheckForCompletedSession(uint64_t queueId);

    /// Populates the specified Kernel statistics structure with info from the specified dispatch packet
    /// \param[in] pAqlPacket the AQL Dispatch packet to get the kernel stats from
    /// \param[in] strAgentName the name of the device the kernel was dispatched to
    /// \param[out] kernelStats the kernelStats for the specified dispatch
    /// \param[in] agent the HSA agent the kernel was dispatched to
    /// \return true on success, false if pAqlPacket is null or if the kernel code object is null
    bool PopulateKernelStatsFromDispatchPacket(const hsa_kernel_dispatch_packet_t* pAqlPacket,
                                               const std::string& strAgentName,
                                               KernelStats& kernelStats,
                                               hsa_agent_t agent);

    /// Add Occupancy entry
    /// \param kernelStats kernel stats structure which contains most of the info we need for occupancy
    /// \param deviceName the name of the device the kernel was dispatched to
    /// \param agent the HSA agent the kernel was dispatched to
    /// \return true if the kernel occupancy info was sucessfully gathered and added, false otherwise
    bool AddOccupancyEntry(const KernelStats& kernelStats, const std::string& deviceName, hsa_agent_t agent);

    /// Default value for Max number of seconds to wait for a session to complete before timing out
    static const uint32_t m_DEFAULT_MAX_SECONDS_TO_WAIT_FOR_SESSIONS = 10;

    /// Typedef for holding a map of active session per queue
    typedef std::unordered_map<uint64_t, SessionInfo> QueueSessionMap;

    std::string             m_strOutputFile;                     ///< Output file
    GPAUtils                m_gpaUtils;                          ///< common GPA utility functions
    std::mutex              m_mtx;                               ///< mutex
    QueueSessionMap         m_activeSessionMap;                  ///< map of active session per queue
    unsigned int            m_uiCurKernelCount;                  ///< number of kernels that have been profiled.
    unsigned int            m_uiMaxKernelCount;                  ///< max number of kernels to profile.
    unsigned int            m_uiOutputLineCount;                 ///< number of items written to the output file
    bool                    m_isProfilingEnabled;                ///< flag indicating if profiling is currently enabled
    bool                    m_isProfilerInitialized;             ///< flag indicating if the profiler object has been initialized already
    bool                    m_bDelayStartEnabled;                ///< flag indicating whether or not the profiler should start with delay or not
    bool                    m_bProfilerDurationEnabled;          ///< flag indiacating whether profiler should only run for certain duration
    unsigned long           m_delayInMilliseconds;               ///< milliseconds to delay for profiler to start
    unsigned long           m_durationInMilliseconds;            ///< duration in milliseconds for which Profiler should run
    ProfilerTimer*          m_pDelayTimer;                       ///< timer for handling delay timer for the profile agent
    ProfilerTimer*          m_pDurationTimer;                    ///< timer for handling duration timer for the profile agent
    GPA_CommandListId       m_commandListId;                     ///< GPA Command List object for the session
#if defined (_LINUX) || defined (LINUX)
    HsaKernelAssembly       m_hsaKernelAssembly;                 ///< manages retrieving the HSA source, IL and ISA from the HSA runtime
#endif
};

#endif  //_HSA_GPA_PROFILE_H_
