//==============================================================================
// Copyright (c) 2015 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief This class provides wrappers for OS-specific functionality
//==============================================================================

#ifndef _OS_UTILS_H_
#define _OS_UTILS_H_

/// \addtogroup Common
// @{

#include "LocalTSingleton.h"
#include "OSDefs.h"
#include "ProfilingParams.h"

typedef void (*TimerCallbackFunc)(TIMERID timerID);
typedef void (*ThreadFunc)(void* param);

typedef ULONGLONG(*GetTimeProc)(void);
typedef bool (*UserTimerInitProc)(void);
typedef void (*UserTimerDestroyProc)(void);


/// structure used when creating threads
struct ThreadFuncWrapperParam
{
    ThreadFunc m_pFunc;   ///< thread function
    void*      m_pParam;  ///< param passed to thread function
};

#ifdef CL_UNITTEST_MOCK
    TSingletonMockGen(OSUtils, OSUtilsMock)
#endif

class OSUtils : public TSingleton<OSUtils>
{
    friend class TSingleton<OSUtils>;
public:

    /// Sets up the user timer based on the specified parameters
    /// \param params the parameters to check
    void SetupUserTimer(const Parameters& params);

    /// Shuts down the user timer
    void ShutdownUserTimer();

    /// Indicates whether or not a user timer is being used
    /// \return true if a user timer is being used, false otherwise
    bool IsUserTimerEnabled() const { return m_bUserTimer; }

    /// Get Current time in nanoseconds
    /// Default time function, but if user selected
    /// time function is loaded, that function is called
    /// \return current time in nanoseconds
    ULONGLONG GetTimeNanos(void);

    /// Function to load a library
    /// \param strFullLibraryName name of library containing function to be called (including path)
    /// \return handle to library module
    LIB_HANDLE GenericLoadLibrary(const std::string& strFullLibraryName);

    /// Function to unload library
    /// \param pLibrary handle to library to be released
    void GenericUnloadLibrary(LIB_HANDLE pLibrary);

    /// Get loaded library handle
    /// \param szLibName library name
    /// \return library handle if libraray is loaded
    LIB_HANDLE GetLibraryHandle(const char* szLibName);

    /// Function to get a pointer to the timer functions in the user library
    /// \param pModule handle to library/module where function is found
    /// \param strFunctionName string containing the name of the function to be called
    /// \return pointer to function; NULL if function not located
    void* GetSymbolAddr(LIB_HANDLE pModule, std::string strFunctionName);

    /// Create a new thread
    /// \param pFunc function pointer
    /// \param param parameters to the new thread
    THREADHANDLE CreateThread(ThreadFunc pFunc, void* param);

    /// Sleep in millisecond
    /// \param milisecond Time in millisecond
    void SleepMillisecond(unsigned int milisecond);

    /// Join thread
    /// \param tid thread id
    int Join(THREADHANDLE tid);

    /// Create process
    /// \param szExe executable path
    /// \param szArgs command line argments, it can' be NULL, the first argment has to be exe path
    /// \param szWorkingDir working directory, not applicable to linux
    /// \param szEnvBlock a zero-separated, double-zero-terminated string containing the environment block.  NULL indicates that it should use the calling process' environment block
    /// \param bCreateSuspended created in suspended state
    /// \param bCreateConsole Windows: create a new console windows; Linux: set to false to suppress stderr/stdout
    /// \return process id(linux) or PROCESS_INFORMATION(windows)
    PROCESSID ExecProcess(const char* szExe, const char* szArgs, const char* szWorkingDir, const char* szEnvBlock, bool bCreateSuspended = false, bool bCreateConsole = true);

    /// Wait for a process to finish
    /// \param pid Process ID
    /// \return TRUE if succeeded
    bool WaitForProcess(PROCESSID pid);

    /// Set environment variable
    /// \param szName Variable name
    /// \param szVal Value
    /// \return true if succeed
    bool SetEnvVar(const char* szName, const char* szVal);

    /// Unset environment variable
    /// \param szName Variable name
    /// \return true if succeed
    bool UnsetEnvVar(const char* szName);

    /// Get environment variable
    /// \param szName Variable name
    /// \return value
    std::string GetEnvVar(const char* szName);

    /// Get the process environment block
    /// \return a pointer to the environment block
    ENVSYSBLOCK GetSysEnvBlock();

    /// Release an env block previously returned by GetSysteEnvBlock
    /// \param pEnvBlock pointer to the block previously returned by GetSysEnvBlock
    void ReleaseSysEnvBlock(ENVSYSBLOCK pEnvBlock);

    /// retrieve the OS information
    /// \return OS version
    std::string GetOSInfo(void);

    /// Copy file
    /// \param szFrom Source path
    /// \param szTo Destination path
    /// \return true if succeed.
    bool OSCopyFile(const char* szFrom, const char* szTo);

    /// Move file
    /// \param szFrom Source path
    /// \param szTo Destination path
    /// \return true if succeed.
    bool OSMoveFile(const char* szFrom, const char* szTo);

protected:
    OSUtils();

private:
#ifdef _WIN32
    double               m_dInvFrequency;          ///< Inverse of timer frequency
#endif

    GetTimeProc          m_pGetUserTime;           ///< Pointer to user timer function: GetTime()
    UserTimerInitProc    m_pUserTimerInit;         ///< Pointer to user timer initialization function: InitTimer()
    UserTimerDestroyProc m_pUserTimerDestroy;      ///< Pointer to user timer destruction function: DestroyTimer()
    bool                 m_bUserTimer;             ///< flag to signal the use of the standard timer (value is false) or a user timer (value is true)
    LIB_HANDLE           m_userTimerLibraryHandle; ///< handle to the user timer library

#if defined(_LINUX) || defined(LINUX)
    /// helper function used by Linux versions of osCopyFile and osMoveFile
    /// \param szFrom Source path
    /// \param szTo Destination path
    /// \param bMove flag indicating whether or not this is a copy or a move
    /// \return true if succeed.
    bool OSCopyMoveFileHelper(const char* szFrom, const char* szTo, bool bMove);
#endif
};

// @}

#endif //_OS_UTILS_H_
