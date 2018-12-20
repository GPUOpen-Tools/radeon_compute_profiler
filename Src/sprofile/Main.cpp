//==============================================================================
// Copyright (c) 2015 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  This is the main command line application that will launch
///         all the agents.
//==============================================================================

#ifdef _WIN32
    #include <windows.h>
    #include "Interceptor.h"
    #include <tchar.h>
    #include <vector>
    #include <string>
#else // LINUX
    #include <sys/wait.h>
    #include <cstdlib>
    #include <cstdio>
    #include <cstring>
#endif

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <signal.h>
#include "ParseCmdLine.h"
#include "Analyze.h"
#include "OccupancyChart.h"
#include "OccupancyUtils.h"
#include "PerfMarkerAtpFile.h"
#include "CSVFileMerger.h"
#include <OSUtils.h>
#include <StringUtils.h>
#include <FileUtils.h>
#include <FileUtilsDefs.h>
#include <StackTraceAtpFile.h>
#include <Version.h>
#include <BinFileHeader.h>
#include <Logger.h>
#include "../CLTraceAgent/CLAtpFile.h"
#include "../HSAFdnTrace/HSAAtpFile.h"
#include "../CLOccupancyAgent/CLOccupancyFile.h"

#include <AMDTOSWrappers/Include/osDirectory.h>
#include <AMDTOSWrappers/Include/osFilePath.h>
#include <AMDTOSWrappers/Include/osEnvironmentVariable.h>
#include <AMDTOSWrappers/Include/osProcess.h>
#include <AMDTOSWrappers/Include/osFile.h>
#include <AMDTOSWrappers/Include/osOutOfMemoryHandling.h>

static Parameters params;
static Config config;
static osProcessId processId;
static gtString strTmpFilePath;
static bool bDoneMerging = false;

static void MergeFragFiles(int sig);
static void MergeTraceFile(int sig);
static void MergeOccupancyFile(int sig);
static bool SetAgent(const gtString& strDirPath);

static const char* ROCP_TOOL_LIB_ENV_VAR_NAME = "ROCP_TOOL_LIB";             ///< The ROCProfiler tool lib env variable name
static const char* ROCP_HSA_INTERCEPT_ENV_VAR_NAME = "ROCP_HSA_INTERCEPT";   ///< The ROCProfiler HSA intercept env variable name
static const char* ROCP_TIMESTAMP_ON_ENV_VAR_NAME = "ROCP_TIMESTAMP_ON_LIB"; ///< The ROCProfiler timestamp on env variable name

#if defined (_LINUX) || defined (LINUX)
    static const char* LD_PRELOAD_ENV_VAR_NAME = "LD_PRELOAD";                   ///< The LD_PRELOAD env variable name
    static bool SetPreLoadLibs();
#endif

static bool CheckIsAppValid(const gtString& strAppName, const int iProfilerNbrBits);

static int  GetNbrAppBits(const gtString& strProfiler);

#ifdef _WIN32
    typedef std::wstring EnvSysBlockString;  ///< type of the system env block: std::wstring on Windows std::string on Linux
    #define ENVBLOCKDELIMITER L'\0'
    #define ENVVARSEPARATOR L'='
#elif defined (_LINUX) || defined (LINUX)
    typedef std::string EnvSysBlockString;   ///< type of the system env block: std::wstring on Windows std::string on Linux
    #define ENVBLOCKDELIMITER '\0'
    #define ENVVARSEPARATOR '='
#endif

/// Gets the environment block to be passed to the profiled program
/// \param mapUserBlock a map of the user-specified environment variables
/// \param bIncludeSystemEnv true if mapUserBlock should augment the system environment block, false if mapUserBlock should replace the system environment block
/// \return a string that can be passed to CreateProcess (or the Linux equivalent)
EnvSysBlockString GetEnvironmentBlock(EnvVarMap mapUserBlock, bool bIncludeSystemEnv);

static bool SetHSAServer(const gtString& strDirPath);

/// Set a maximum number of agents to be supported
const unsigned int MAX_NBR_AGENTS = 5;

///Constant strings
#define CL_AGENT_OCCUPANCY  GPU_PROFILER_LIBRARY_NAME_PREFIX "CLOccupancyAgent"
#define CL_AGENT_TRACE      GPU_PROFILER_LIBRARY_NAME_PREFIX "CLTraceAgent"
#define CL_AGENT_PERF_CTR   GPU_PROFILER_LIBRARY_NAME_PREFIX "CLProfileAgent"
#define CL_AGENT_SUB_KRNL   GPU_PROFILER_LIBRARY_NAME_PREFIX "CLSubKernelProfileAgent"
#define LOG_FILE_NAME   "rcprof"
#define LOG_FILE_EXTENSION ".log"

#ifdef _WIN32

// print last error from the system
static bool PrintLastError(wchar_t* szPre)
{
    wchar_t szError[1024];
    DWORD dwError = GetLastError();

    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
                  NULL,
                  dwError,
                  0,
                  szError,
                  1024,
                  NULL);

    std::cout << szPre << ": "
              << dwError << ": " << szError << std::endl;

    return true;
}

static int CreateProcessWithDetour(gtString& strDirPath, gtString& strAppCommandLine, gtString& strAppWorkingDirectory, bool useDetours)
{
    // set the detoured and microDLL server's path
    std::string dirPathAsUTF8;
    StringUtils::WideStringToUtf8String(strDirPath.asCharArray(), dirPathAsUTF8);

    char szMicroDllPath[ MAX_PATH ];
    SP_strcpy(szMicroDllPath, MAX_PATH, dirPathAsUTF8.c_str());
    SP_strcat(szMicroDllPath, MAX_PATH, MICRO_DLL);

    // Use Detours to launch the app and load our OpenCL server into the process
    STARTUPINFO si;
    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);

    PROCESS_INFORMATION pi;
    memset(&pi, 0, sizeof(pi));

    LPVOID pEnvBlock = NULL;

    EnvSysBlockString strEnvBlock;

    if (!config.mapEnvVars.empty())
    {
        strEnvBlock = GetEnvironmentBlock(config.mapEnvVars, !config.bFullEnvBlock);

        if (!strEnvBlock.empty())
        {
            pEnvBlock = (LPVOID)strEnvBlock.c_str();
        }
    }

    BOOL createProcRetVal = FALSE;

    if (useDetours)
    {
        // Run the app with MicroDLL enabled
        createProcRetVal = AMDT::CreateProcessAndInjectDllW(config.strInjectedApp.asCharArray(),
                                                            (LPWSTR)strAppCommandLine.asCharArray(),
                                                            NULL, NULL, TRUE,
                                                            CREATE_SUSPENDED | CREATE_DEFAULT_ERROR_MODE | CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT,
                                                            pEnvBlock,
                                                            strAppWorkingDirectory.asCharArray(),
                                                            &si,
                                                            &pi,
                                                            szMicroDllPath);
    }
    else
    {
        createProcRetVal = CreateProcess(config.strInjectedApp.asCharArray(),
                                         (LPWSTR)strAppCommandLine.asCharArray(),
                                         NULL, NULL, TRUE,
                                         CREATE_SUSPENDED | CREATE_DEFAULT_ERROR_MODE | CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT,
                                         pEnvBlock,
                                         strAppWorkingDirectory.asCharArray(),
                                         &si,
                                         &pi);
    }

    if (!createProcRetVal)
    {
        PrintLastError(L"Failed to start application");
        return -1;
    }

    // On Windows, always set processId > 0 so that we can do merging
    processId = pi.dwProcessId;

    // Resume thread and wait on the process..
    if (ResumeThread(pi.hThread) == (DWORD) - 1)
    {
        PrintLastError(L"Failed to resume thread");
        return -1;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD exitCode;

    if (config.bTestMode && GetExitCodeProcess(pi.hProcess, &exitCode))
    {
        // if process returned an error code, return that error code from rcprof
        if (exitCode != 0)
        {
            return exitCode;
        }
    }

    return 0;
}

#endif

std::string GetExpectedOutputFile(const std::string& strOutputFileArg, const std::string& strReqdExtension)
{
    std::string strExtension = FileUtils::GetFileExtension(strOutputFileArg);
    std::string strProfileOutputFile("");

    if (strExtension == strReqdExtension)
    {
        strProfileOutputFile = strOutputFileArg;
        return strProfileOutputFile;
    }
    else
    {

        if ((strExtension == TRACE_EXT) ||
            (strExtension == OCCUPANCY_EXT) ||
            (strExtension == PERF_COUNTER_EXT))
        {
            strProfileOutputFile = FileUtils::GetBaseFileName(strOutputFileArg);
            strProfileOutputFile += ".";
            strProfileOutputFile += strReqdExtension;
            return strProfileOutputFile;
        }
        else
        {
            strProfileOutputFile = strOutputFileArg + ".";
            strProfileOutputFile += strReqdExtension;
            return strProfileOutputFile;
        }
    }
}

void CheckOutputFile(const Config& configInner)
{
    std::string strOutputFile("");
    std::string strRequiredExt("");

    if (configInner.bTrace || configInner.bHSATrace || configInner.bMergeMode)
    {
        strRequiredExt.assign(TRACE_EXT);
        strOutputFile = GetExpectedOutputFile(configInner.strOutputFile, strRequiredExt);

        if (FileUtils::FileExist(strOutputFile))
        {
            std::cout << "Session output path: " << strOutputFile << std::endl;
        }
        else
        {
            std::cout << "Failed to generate profile result " << strOutputFile << "." << std::endl;
        }
    }

    if (configInner.bOccupancy && !configInner.bHSATrace)
    {
        strRequiredExt.assign(OCCUPANCY_EXT);

        std::string occupancyFile = configInner.strOutputFile;

        if ((configInner.bHSAPMC || configInner.bPerfCounter) && configInner.counterFileList.size() > 1)
        {
            size_t passStringPosition = config.strOutputFile.find("_pass");

            if (passStringPosition != std::string::npos)
            {
                //Remove the appended "_pass"" string and the extension
                occupancyFile = config.strOutputFile.substr(0, passStringPosition);
            }
        }

        strOutputFile = GetExpectedOutputFile(occupancyFile, strRequiredExt);

        if (FileUtils::FileExist(strOutputFile))
        {
            std::cout << "Session output path: " << strOutputFile << std::endl;
        }
        else
        {
            std::cout << "Failed to generate profile result " << strOutputFile << "." << std::endl;
        }
    }

    if (configInner.bPerfCounter || configInner.bHSAPMC)
    {
        strRequiredExt.assign(PERF_COUNTER_EXT);
        strOutputFile = GetExpectedOutputFile(configInner.strOutputFile, strRequiredExt);

        if (FileUtils::FileExist(strOutputFile))
        {
            std::cout << "Session output path: " << strOutputFile << std::endl;
        }
        else
        {
            std::cout << "Failed to generate profile result " << strOutputFile << "." << std::endl;
        }
    }
}

int DisplayOccupancy(const std::string& strOutputFile)
{
    int retVal = 0;

    if (!config.strOccupancyParamsFile.empty())
    {
        OccupancyUtils::OccupancyParams paramsInner;

        std::string occupancyError;

        bool occupancyParamsRetrieved = false;

        if (UNSPECIFIED_OCCUPANCY_INDEX == config.uiOccupancyIndex)
        {
            occupancyParamsRetrieved = OccupancyUtils::GetOccupancyParamsFromFile(config.strOccupancyParamsFile, paramsInner, occupancyError);
        }
        else
        {
            occupancyParamsRetrieved = OccupancyUtils::GetOccupancyParamsFromFile(config.strOccupancyParamsFile, config.uiOccupancyIndex, paramsInner, occupancyError);
        }

        //Generate HTML file
        if (occupancyParamsRetrieved && GenerateOccupancyChart(paramsInner, strOutputFile, occupancyError))
        {
            retVal = 0;
        }
        else
        {
            std::cout << "Error generating occupancy display file." << std::endl << occupancyError << std::endl;
            retVal = -1;
        }
    }

    return retVal;
}

/// Sets the GPU Stable clock mode
/// \param mode the clock mode to use: 0 is default
void SetStableClocks(unsigned int mode)
{
    if (!config.bNoStableClocks && config.bPerfCounter)
    {
#ifdef _WIN32
        const char* pStableClocksExe = "VkStableClocks-x64.exe";
        std::stringstream args;
        args << FileUtils::GetExePath().c_str() << "/" << pStableClocksExe << " " << mode;
        PROCESSID pid = OSUtils::Instance()->ExecProcess(nullptr, args.str().c_str(), nullptr, nullptr, false, false);
#else
        const char* pStableClocksExe = "VkStableClocks";
        std::stringstream exe;
        std::stringstream args;
        exe << FileUtils::GetExePath().c_str() << "/" << pStableClocksExe;
        args << mode;
        PROCESSID pid = OSUtils::Instance()->ExecProcess(exe.str().c_str(), args.str().c_str(), nullptr, nullptr, false, false);
#endif

        OSUtils::Instance()->WaitForProcess(pid);
    }
}

bool SetROCPInterceptEnvVar(bool showMessage, std::string interceptionLevel)
{
    SP_UNREFERENCED_PARAMETER(showMessage);
    bool retVal = true;

#ifndef _WIN32
    retVal = OSUtils::Instance()->SetEnvVar(ROCP_HSA_INTERCEPT_ENV_VAR_NAME, interceptionLevel.c_str());

    if (showMessage)
    {
        if (!retVal)
        {
            std::cout << "Error: Unable to enable queue interception in rocprofiler library\n";
        }
        else
        {
            std::cout << "Successfully enabled queue interception in rocprofiler library\n";
        }
    }

#endif
    return retVal;
}

bool UnsetROCPInterceptEnvVar(bool showMessage)
{
    SP_UNREFERENCED_PARAMETER(showMessage);
    bool retVal = true;

#ifndef _WIN32
    retVal = OSUtils::Instance()->UnsetEnvVar(ROCP_HSA_INTERCEPT_ENV_VAR_NAME);

    if (showMessage)
    {
        if (!retVal)
        {
            std::cout << "Error: Unable to disable queue interception in rocprofiler library\n";
        }
        else
        {
            std::cout <<  "Successfully disabled queue interception in rocprofiler library\n";
        }
    }
#endif

    return retVal;
}

int ProfileApplication(const std::string& strCounterFile, const int& profilerBits)
{
    int retVal = 0;

    std::string counterfile = strCounterFile;

    if ((config.bTrace || config.bHSATrace) && config.bAnalyze && config.analyzeOps.strAtpFile.empty())
    {
        // use trace output as sanalyze module input
        config.analyzeOps.strAtpFile = config.strOutputFile;
    }

    params.m_strOutputFile = config.strOutputFile;
    params.m_strSessionName = config.strSessionName;

    // Note: the following is a fix for CODEXL-50 -- use osDirectory to create the output directory
    //       if it does not already exist.  Some of the code here can be removed when we move to
    //       use osFilePath/gtString in more places in the backend (i.e. like in all members in the
    //       "Config" sturcture declared in ParseCmdLine.h
    gtString gtStringOutputFile;
    gtStringOutputFile.fromUtf8String(params.m_strOutputFile.c_str());
    osFilePath outputFilePath(gtStringOutputFile);
    osDirectory outputDir;
    outputFilePath.getFileDirectory(outputDir);

    if (!outputDir.exists() && !outputDir.asFilePath().isEmpty())
    {
        outputDir.create();
    }

    // Note: end fix for CODEXL-50

    //----------------------------------------
    // Merge mode
    //----------------------------------------

    if (config.bMergeMode)
    {
        std::cout << "--- Merge Mode ---" << std::endl;
        std::cout << "Temp files prefix (Process ID): " << config.uiPID << std::endl;
        processId = config.uiPID;

        if (config.strWorkingDirectory.isEmpty())
        {
            osFilePath tempPath;
            tempPath.setPath(osFilePath::OS_CURRENT_DIRECTORY);
            strTmpFilePath = tempPath.asString();
        }
        else
        {
            strTmpFilePath = config.strWorkingDirectory;
        }

        config.bTrace = true;
        config.bHSATrace = true;
        params.m_bTimeOutBasedOutput = true;
        MergeFragFiles(1);

        return 0;
    }

    //----------------------------------------
    // Remove all tmp files
    //----------------------------------------
    FileUtils::RemoveFragFiles();

    //----------------------------------------
    // Get rcprof.exe's full path
    //----------------------------------------
    gtString strDirPath = FileUtils::GetExePathAsUnicode();

#if defined (_LINUX) || defined (LINUX)
    {
        //----------------------------------------
        // Set replace tilde
        //----------------------------------------
        gtString retVal;
        osGetCurrentProcessEnvVariableValue(L"HOME", retVal);
        std::string strHomePath;
        StringUtils::WideStringToUtf8String(retVal.asCharArray(), strHomePath);

        FileUtils::ReplaceTilde(strHomePath, config.strOutputFile);
        FileUtils::ReplaceTilde(strHomePath, counterfile);

        // replace tilde using gtString
        if (config.strInjectedApp[0] == '~')
        {
            config.strInjectedApp.extruct(0, 1);
            gtString tempStr = config.strInjectedApp;
            config.strInjectedApp = retVal;
            config.strInjectedApp.appendFormattedString(L"%ls", tempStr.asCharArray());
        }

        // For linux, we need to check file existence before we fork
        osFile fileToCheck(config.strInjectedApp);

        if (!fileToCheck.exists())
        {
            std::cout << "Process failed to run. Make sure you have specified the correct path." << std::endl;
            return -1;
        }
    }
#endif

    bool bAnyAgentSet = false;

    //----------------------------------------
    // Set Agent
    //----------------------------------------
    if (config.bHSATrace || config.bHSAPMC)
    {
        bAnyAgentSet |= SetHSAServer(strDirPath);
    }

    bAnyAgentSet |= SetAgent(strDirPath);

    if (!bAnyAgentSet)
    {
        if (!config.bAnalyzeOnly && !config.bMergeMode)
        {
            std::cout << "No profile mode specified. Nothing will be done." << std::endl;
        }

        return 1;
    }

    //----------------------------------------
    // Pass params
    //----------------------------------------

    params.m_strCmdArgs = config.strInjectedAppArgs;
    params.m_strWorkingDir = config.strWorkingDirectory;
    params.m_strCounterFile = counterfile;
    params.m_strKernelFile = config.strKernelFile;
    params.m_strAPIFilterFile = config.strAPIFilterFile;
    params.m_strDLLPath = strDirPath;
    params.m_cOutputSeparator = config.cOutputSeparator;
    params.m_bVerbose = config.bVerbose;
    params.m_bPerfCounter = config.bPerfCounter;
    params.m_bOutputIL = config.bOutputIL;
    params.m_bOutputISA = config.bOutputISA;
    params.m_bOutputCL = config.bOutputCL;
    params.m_bOutputHSAIL = config.bOutputHSAIL;
    params.m_bTrace = config.bTrace;
    params.m_bTimeOutBasedOutput = config.bTimeOut;
    params.m_uiTimeOutInterval = config.uiTimeOutInterval;
    params.m_bTestMode = config.bTestMode;
    params.m_bQueryRetStat = config.bQueryRetStat;
    params.m_bCollapseClGetEventInfo = config.bCollapseClGetEventInfo;
    params.m_bUserTimer = config.bUserTimer;
    params.m_strTimerDLLFile = config.strTimerDLLFile;
    params.m_strUserTimerFn = config.strUserTimerFn;
    params.m_strUserTimerInitFn = config.strUserTimerInitFn;
    params.m_strUserTimerDestroyFn = config.strUserTimerDestroyFn;
    params.m_bStackTrace = config.bSym;
    params.m_uiMaxNumOfAPICalls = config.uiMaxNumOfAPICalls;
    params.m_uiMaxKernels = config.uiMaxKernels;
    params.m_bKernelOccupancy = config.bOccupancy;
    params.m_bUserPMC = config.bUserPMCSampler;
    params.m_bCompatibilityMode = config.bCompatibilityMode;
    params.m_strUserPMCLibPath = config.strUserPMCLibPath;
    params.m_bHSATrace = config.bHSATrace;
    params.m_bHSAPMC = config.bHSAPMC;
    params.m_bGMTrace = config.bGMTrace;
    params.m_mapEnvVars = config.mapEnvVars;
    params.m_bFullEnvBlock = config.bFullEnvBlock;
    params.m_bForceSinglePassPMC = config.bForceSinglePassPMC;
    params.m_bGPUTimePMC = config.bGPUTimePMC;
    params.m_bStartDisabled = config.bStartDisabled;
    params.m_delayInMilliseconds = config.uiDelayInMilliseconds > 0 ? config.uiDelayInMilliseconds : 0;
    params.m_bDelayStartEnabled = config.uiDelayInMilliseconds > 0;
    params.m_durationInMilliseconds = config.uiDurationInMilliseconds > 0 ? config.uiDurationInMilliseconds : 0;
    params.m_bProfilerDurationEnabled = config.uiDurationInMilliseconds > 0;
    params.m_bForceSingleGPU = config.bForceSingleGPU;
    params.m_uiForcedGpuIndex = config.uiForcedGpuIndex;
    params.m_bAqlPacketTracing = config.bAqlPacketTracing;
    params.m_bDisableKernelDemangling = config.bDisableKernelDemangling;
    params.m_bNoHSATransferTime = config.bNoHSATransferTime;

#ifdef AMDT_INTERNAL

    if ((params.m_bPerfCounter || params.m_bHSAPMC) && params.m_strCounterFile.empty())
    {
        std::cout << "A counter file must be specified when collecting perf counters in the internal build\n";
        std::cout << "Use --counterfile (or -c) to specify a counter file\n";
        return -1;
    }

#endif

    //for debugging
    //cout << strDirPath << endl;
    //cout << config.strInjectedApp << endl;
    //cout << params.m_strOutputFile << endl;

    FileUtils::PassParametersByFile(params);
    remove(FileUtils::GetCLICDTableFile().c_str());

    //----------------------------------------
    // Get App working dir
    //----------------------------------------

    gtString strAppWorkingDirectory;

    if (config.strWorkingDirectory.isEmpty())
    {
        osFilePath injectedApp(config.strInjectedApp);
        // remove file name and ext:
        injectedApp.setFileName(L"");
        injectedApp.setFileExtension(L"");
        strAppWorkingDirectory = injectedApp.asString();
        // FileUtils::GetWorkingDirectory(config.strInjectedApp, strAppWorkingDirectory);
    }
    else
    {
        strAppWorkingDirectory = config.strWorkingDirectory;
    }


    //----------------------------------------
    // Set signal
    //----------------------------------------
    if (config.bTrace || config.bHSATrace || config.bOccupancy || config.bThreadTrace)
    {
        // set tmp file path
        strTmpFilePath = FileUtils::GetTempFragFilePathAsUnicode();

        signal(SIGABRT, MergeFragFiles);
        signal(SIGTERM, MergeFragFiles);
        signal(SIGINT, MergeFragFiles);
    }

    //----------------------------------------
    // Create process
    //----------------------------------------

    //check that the application to be profiled is valid
    if (!CheckIsAppValid(config.strInjectedApp, profilerBits))
    {
        std::wcout << config.strInjectedApp.asCharArray() << " is not a valid application" << std::endl;
        return -1;
    }

#ifdef _DEBUG
    bool reportPerfCounterEnablement = true;
#else
    bool reportPerfCounterEnablement = false;
#endif

    if (config.bHSAPMC)
    {
        SetROCPInterceptEnvVar(reportPerfCounterEnablement, "1");
    }
    else if (config.bHSATrace)
    {
        SetROCPInterceptEnvVar(reportPerfCounterEnablement, "2");
    }

#ifdef _WIN32

    gtString strAppCommandLine;
    strAppCommandLine.appendFormattedString(L"\"%ls\"", config.strInjectedApp.asCharArray());

    // create a command line if the app argument list is not empty
    // put arguments in quotes
    if (!config.strInjectedAppArgs.isEmpty())
    {
        strAppCommandLine.appendFormattedString(L" %ls", config.strInjectedAppArgs.asCharArray());
    }

    int ret = CreateProcessWithDetour(strDirPath, strAppCommandLine, strAppWorkingDirectory, !config.bNoDetours);

    if (ret != 0)
    {
        FileUtils::DeleteTmpFile();
        return -1;
    }

#else

    if (!config.bNoStableClocks && config.bPerfCounter)
    {
        // For OpenCL perf counter collection on most Linux driver
        // stacks, write access is required on a particular system
        // file in order to set stable GPU clocks.
        // This code checks if the system files are writeable and
        // issues a message if not.
        std::wstringstream wss;
        gtString dpmBaseDir = L"/sys/class/drm/card";
        gtString dpmRestOfPath = L"device";
        gtString dpmFile = L"power_dpm_force_performance_level";

        bool fileFound = true;
        unsigned int cardIndex = 0;

        while (fileFound)
        {
            wss.str(L"");
            wss << dpmBaseDir.asCharArray() << cardIndex << L"/";
            cardIndex++;
            gtString baseDir = wss.str().c_str();;
            osFilePath baseFilePath(baseDir);

            if (baseFilePath.isDirectory())
            {
                baseFilePath.appendSubDirectory(dpmRestOfPath);
                baseFilePath.setFileName(dpmFile);

                if (baseFilePath.isRegularFile())
                {
                    osFile sysFile;

                    // check if the file can be opened for writing
                    if (!sysFile.open(baseFilePath, osChannel::OS_ASCII_TEXT_CHANNEL, osFile::OS_OPEN_TO_WRITE))
                    {
                        // if not suggest to the user to either run as root or modify permissions on the file
                        std::cout << "\nInsufficient privileges. Either re-run as root or modify the permissions on\n"
                                  << baseFilePath.asString().asASCIICharArray() << std::endl
                                  << "to give the current user write access.\n\n";

                        return -1;
                    }
                    else
                    {
                        sysFile.close();
                    }
                }
                else
                {
                    fileFound = false;
                }
            }
            else
            {
                fileFound = false;
            }
        }
    }

    SetPreLoadLibs();

    std::string strAppCommandLine;

    // create a command line if the app argument list is not empty
    // put arguments in quotes
    size_t nCmdlineLength = 0;

    if (!config.strInjectedAppArgs.isEmpty())
    {
        StringUtils::WideStringToUtf8String(config.strInjectedAppArgs.asCharArray(), strAppCommandLine);
        nCmdlineLength = strAppCommandLine.length();
    }

    char* pszCmdline = new(std::nothrow) char[nCmdlineLength + 1];

    if (pszCmdline == NULL)
    {
        std::cout << "Error processing command line\n";
        return -1;
    }

    if (nCmdlineLength > 0)
    {
        strcpy(pszCmdline, strAppCommandLine.c_str());
    }
    else
    {
        pszCmdline[0] = '\0';
    }

    char szExe[SP_MAX_PATH] = { '\0' };
    std::string convertedInjectApp;
    StringUtils::WideStringToUtf8String(config.strInjectedApp.asCharArray(), convertedInjectApp);

    strcpy(szExe, convertedInjectApp.c_str());

    const char* pEnvBlock = NULL;

    EnvSysBlockString strEnvBlock;

    if (!config.mapEnvVars.empty())
    {
        strEnvBlock = GetEnvironmentBlock(config.mapEnvVars, !config.bFullEnvBlock);

        if (!strEnvBlock.empty())
        {
            pEnvBlock = strEnvBlock.c_str();
        }
    }

    std::string convertedWorkingDir;
    StringUtils::WideStringToUtf8String(strAppWorkingDirectory.asCharArray(), convertedWorkingDir);

    processId = OSUtils::Instance()->ExecProcess(szExe, pszCmdline, convertedWorkingDir.c_str(), pEnvBlock);

    if (processId < 0)
    {
        // error
        processId = 0;
        std::cout << "error in fork()\n";
        exit(1);
    }
    else if (processId > 0)
    {
        // parent code
        int status;
        waitpid(processId, &status, 0);

        // if process returned an error, return that error code from rcprof
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
        {
            retVal = WEXITSTATUS(status);
        }

        // if process was terminated by a signal, return that signal number from rcprof
        if (WIFSIGNALED(status) && WTERMSIG(status) != 0)
        {
            retVal = WTERMSIG(status);
        }
    }

    delete[] pszCmdline;
#endif
    // Work-around an OpenCL driver issue where it can leave the GPU
    // in stable clock mode after collecting performance counters
    SetStableClocks(0);

    if (config.bHSAPMC)
    {
        UnsetROCPInterceptEnvVar(reportPerfCounterEnablement);
    }

    if (config.bHSATrace)
    {
        UnsetROCPInterceptEnvVar(reportPerfCounterEnablement);
    }

    //----------------------------------------
    // Unset agent before calling CLUtils to generate atp file header
    //----------------------------------------
    OSUtils::Instance()->UnsetEnvVar(OCL_ENABLE_PROFILING_ENV_VAR);

    if (config.bHSATrace)
    {
        OSUtils::Instance()->UnsetEnvVar(HSA_ENABLE_PROFILING_ENV_VAR);
    }

    //----------------------------------------
    // Merge result if needed
    //----------------------------------------

    MergeFragFiles(1);
    CheckOutputFile(config);

    return retVal;
}

int ProcessCommandLine(const std::string& strCounterFile)
{
    int retVal = 0;

    //If the occupancy switch is set, open the occupancy parameters file and parse
    //Then, generate the HTML output.
    if (config.bOccupancyDisplay)
    {
        if (!config.strOccupancyParamsFile.empty())
        {
            return DisplayOccupancy(config.strOutputFile);
        }
    }

    if (!config.bAnalyzeOnly)
    {
        //Get the number of bits of the profiler
        int iProfilerNbrBits = FileUtils::FILE_BITS_UNKNOWN;

        gtString strProfiler = FileUtils::GetExeFullPathAsUnicode();

        iProfilerNbrBits = GetNbrAppBits(strProfiler);
        retVal = ProfileApplication(strCounterFile, iProfilerNbrBits);
    }


    //----------------------------------------
    // Summary
    //----------------------------------------
    if (config.bAnalyze)
    {
        if (!APITraceAnalyze(config))
        {
            std::cout << "\nFailed to generate summary pages\n";
            retVal = -1;
        }
    }

    //----------------------------------------
    // Cleanup
    //----------------------------------------

    FileUtils::DeleteTmpFile();

    return retVal;
}

bool MergeKernelProfileOutputFiles(std::vector<std::string> counterFileList,
                                   std::vector<std::string> outputFileList,
                                   std::string defaultOutputFileName,
                                   GPA_API_Type apiName,
                                   bool includeTime)
{
    bool isOutputFileExist = true;

    for (std::vector<std::string>::iterator it = outputFileList.begin(); it != outputFileList.end(); ++it)
    {
        isOutputFileExist &= FileUtils::FileExist(*it);
    }

    if (!isOutputFileExist)
    {
        std::cout << "Profiling files are not generated. No Merging required.\n\n";
    }
    else
    {
        if (outputFileList.size() > 1 && counterFileList.size() > 1)
        {
            std::map<unsigned int, KernelRowData*> dataPerFile;
            std::vector<CSVFileParser*> csvFileParsers;

            // headers (or comments) in the csv file
            std::vector<std::string> headers;

            unsigned int count = 0;

            // Load all the CSV files in Kernel Row Data
            for (std::vector<std::string>::iterator it = outputFileList.begin(); it != outputFileList.end(); ++it)
            {
                CSVFileParser* csvParser = new(std::nothrow) CSVFileParser;
                KernelRowData* rowData = new(std::nothrow) KernelRowData;

                if (nullptr != csvParser && nullptr != rowData)
                {
                    csvFileParsers.push_back(csvParser);
                    csvParser->AddListener(rowData);

                    if (csvParser->LoadFile(it->c_str()) && csvParser->Parse())
                    {
                        dataPerFile.insert(std::pair<unsigned int, KernelRowData*>(count, rowData));
                        count++;
                    }

                    // header will be same in all files - retreiving from 1st file
                    headers = csvParser->GetHeaders();
                }
            }

            std::string collatedOutputFileName;

            if (FileUtils::GetFileExtension(defaultOutputFileName).empty())
            {
                collatedOutputFileName = defaultOutputFileName + "." + PERF_COUNTER_EXT;
            }
            else
            {
                collatedOutputFileName = defaultOutputFileName;
            }

            CSVFileWriter mergedFileWriter(collatedOutputFileName);
            std::map<std::string, std::vector<int>> counterColumns;
            std::map<std::string, std::vector<int>>::iterator counterColumnsIterator;
            std::vector<std::string> csvFileColumns;
            std::string passString = "_pass_";

            HeaderList headersWithFileIndex = KernelRowDataHelper::CreateHeader(counterFileList, apiName, includeTime);
            HeaderList::iterator headersWithFileIndexIterator;

            for (headersWithFileIndexIterator = headersWithFileIndex.begin(); headersWithFileIndexIterator != headersWithFileIndex.end(); ++headersWithFileIndexIterator)
            {
                csvFileColumns.push_back(StringUtils::Trim(headersWithFileIndexIterator->first));
            }

            for (std::vector<std::string>::const_iterator headerIter = headers.begin(); headerIter != headers.end(); ++headerIter)
            {
                mergedFileWriter.AddHeader(StringUtils::Trim(*headerIter));
            }

            mergedFileWriter.AddColumns(csvFileColumns);

            unsigned int addedRow = 0;
            MappedThreadSetList mappedThreads = KernelRowDataHelper::GetMappedThreads(dataPerFile);
            MappedThreadSetList::iterator mappedThreadsIterator;

            for (mappedThreadsIterator = mappedThreads.begin(); mappedThreadsIterator != mappedThreads.end(); ++mappedThreadsIterator)
            {
                if (mappedThreadsIterator->size() > 1)
                {
                    unsigned int rowsToAdd = dataPerFile[mappedThreadsIterator->begin()->second]->GetRowCountByThreadId(mappedThreadsIterator->begin()->first);
                    unsigned int commonColumnsFileIndex = mappedThreadsIterator->begin()->second;
                    std::string commonThreadId = mappedThreadsIterator->begin()->first;
                    unsigned int fileIndexCounter = 0;

                    for (MappedThreadSet::iterator mappedThreadSetIter = mappedThreadsIterator->begin();
                         mappedThreadSetIter != mappedThreadsIterator->end(); ++mappedThreadSetIter)
                    {
                        unsigned int currentMappedThreadIteratorRowCount = dataPerFile[mappedThreadSetIter->second]->GetRowCountByThreadId(mappedThreadSetIter->first);

                        if (rowsToAdd < currentMappedThreadIteratorRowCount)
                        {
                            rowsToAdd = currentMappedThreadIteratorRowCount;
                            commonColumnsFileIndex = mappedThreadSetIter->second;
                            commonThreadId = mappedThreadSetIter->first;
                        }

                        fileIndexCounter++;
                    }

                    for (unsigned int rowsToAddIter = 0; rowsToAddIter < rowsToAdd; ++rowsToAddIter)
                    {
                        CSVRow* rowToAddToFile = mergedFileWriter.AddRow();
                        addedRow++;

                        MappedThreadSet::iterator tempMappedThreadSetIterator;

                        for (headersWithFileIndexIterator = headersWithFileIndex.begin(); headersWithFileIndexIterator != headersWithFileIndex.end(); ++headersWithFileIndexIterator)
                        {
                            if (KernelRowDataHelper::IsCommonColumn(headersWithFileIndexIterator->first))
                            {
                                rowToAddToFile->SetRowData(headersWithFileIndexIterator->first, dataPerFile[commonColumnsFileIndex]->GetValueByThreadId(commonThreadId, rowsToAddIter, headersWithFileIndexIterator->first));
                            }
                            else
                            {
                                for (tempMappedThreadSetIterator = mappedThreadsIterator->begin(); tempMappedThreadSetIterator != mappedThreadsIterator->end(); ++tempMappedThreadSetIterator)
                                {
                                    if (headersWithFileIndexIterator->second.second == tempMappedThreadSetIterator->second)
                                    {
                                        unsigned int fileIndexValue = tempMappedThreadSetIterator->second;
                                        rowToAddToFile->SetRowData(headersWithFileIndexIterator->first, dataPerFile[fileIndexValue]->GetValueByThreadId(tempMappedThreadSetIterator->first, rowsToAddIter, headersWithFileIndexIterator->second.first));
                                    }
                                }
                            }
                        }
                    }
                }
                else
                {
                    CSVRow* rowToAddToFile = mergedFileWriter.AddRow();
                    addedRow++;
                    MappedThreadSet::iterator tempMappedThreadSetIterator;
                    unsigned int rowIndex = 0;

                    for (headersWithFileIndexIterator = headersWithFileIndex.begin(); headersWithFileIndexIterator != headersWithFileIndex.end(); ++headersWithFileIndexIterator)
                    {
                        if (KernelRowDataHelper::IsCommonColumn(headersWithFileIndexIterator->first))
                        {
                            tempMappedThreadSetIterator = mappedThreadsIterator->begin();
                            std::string checkVal = dataPerFile[tempMappedThreadSetIterator->second]->GetValueByThreadId(tempMappedThreadSetIterator->first, rowIndex, headersWithFileIndexIterator->first);
                            rowToAddToFile->SetRowData(headersWithFileIndexIterator->first, dataPerFile[tempMappedThreadSetIterator->second]->GetValueByThreadId(tempMappedThreadSetIterator->first, 0, headersWithFileIndexIterator->first));
                        }
                        else
                        {
                            if (headersWithFileIndexIterator->second.second == tempMappedThreadSetIterator->second)
                            {
                                unsigned int fileIndexValue = tempMappedThreadSetIterator->second;
                                rowToAddToFile->SetRowData(headersWithFileIndexIterator->first, dataPerFile[fileIndexValue]->GetValueByThreadId(tempMappedThreadSetIterator->first, rowIndex, headersWithFileIndexIterator->second.first));
                            }
                        }
                    }
                }
            }

            bool inconsistentDispatchInInputFiles = false;

            for (std::map<unsigned int, KernelRowData*>::iterator dataPerFileIter = dataPerFile.begin(); dataPerFileIter != dataPerFile.end(); ++dataPerFileIter)
            {
                inconsistentDispatchInInputFiles |= (addedRow > dataPerFileIter->second->GetRowCount());
            }

            if (inconsistentDispatchInInputFiles)
            {
                std::cout << "\n\nUnable to merge all of the data for undeterministic dispatches\n\n";
            }

            std::cout << "\n\nCollated file output path : " << collatedOutputFileName << std::endl << std::endl;
            return mergedFileWriter.Flush();
        }
    }

    return false;
}

#ifdef _WIN32
    int _tmain(int argc, wchar_t* argv[])
#else
    int main(int argc, char* argv[])
#endif
{
    int retVal = 0;

    std::string strLogFileWithPath = FileUtils::GetDefaultOutputPath() + LOG_FILE_NAME + LOG_FILE_EXTENSION;
    GPULogger::LogFileInitialize(strLogFileWithPath.c_str());

    // First, register the out-of-memory event handler.
    std::set_new_handler(osDumpCallStackAndExit);

#ifdef _WIN32

    if (!ParseCmdLine(argc, argv, config))
    {
        return -1;
    }

#else
    std::vector<wchar_t*> convertedArg;
    convertedArg.reserve(argc);
    std::wstring wargs[argc];

    for (int nArg = 0 ; nArg < argc ; nArg++)
    {
        StringUtils::Utf8StringToWideString(argv[nArg], wargs[nArg]);
        convertedArg.push_back(const_cast<wchar_t*>(wargs[nArg].c_str()));
    }

    if (!ParseCmdLine(argc, convertedArg.data(), config))
    {
        return -1;
    }

#endif

    bool isCounterFileMoreThanOne = config.counterFileList.size() > 1 ? true : false;
    std::string passedOutputFileName = config.strOutputFile;
    std::string defaultOutputFileName;
    std::vector<std::string> outputFileList;

    bool needReplay = config.counterFileList.size() > 1;
    const std::string messageOnMultipleSwitches = "\nMultiple Counters cannot be used with trace option and Performance counter mode. Enabling only trace.\n\n";

    if (config.bPerfCounter && config.bTrace)
    {
        config.bPerfCounter = false;

        if (needReplay)
        {
            std::cout << messageOnMultipleSwitches;
        }
    }

    if (config.bHSAPMC && (config.bHSATrace || config.bAqlPacketTracing))
    {
        config.bHSAPMC = false;

        if (needReplay)
        {
            std::cout << messageOnMultipleSwitches;
        }
    }

    if ((config.bHSAPMC || config.bPerfCounter) && needReplay)
    {
        bool isReplaying = false;

        for (unsigned int i = 0; i < config.counterFileList.size(); ++i)
        {
            std::string outputFileName;
            std::string logFile;
            std::string appendString;

            if (!outputFileName.empty())
            {
                outputFileName.clear();
            }

            if (isReplaying)
            {
                config.bTrace = false;
                config.bHSATrace = false;
                config.bMergeMode = false;
                config.bSubKernelProfile = false;
                config.bThreadTrace = false;
                config.bOccupancy = false;
                config.bAqlPacketTracing = false;
            }

            //Output File
            //----------------------------------------
            // Get output file path
            //----------------------------------------
            if (passedOutputFileName.empty())
            {
                if (isCounterFileMoreThanOne)
                {
                    std::stringstream stringStream;
                    stringStream << "_pass" << (i + 1);
                    appendString = stringStream.str() ;
                }

                if (config.bPerfCounter || config.bHSAPMC)
                {
                    defaultOutputFileName = FileUtils::GetDefaultProfileOutputFile();
                    outputFileName = FileUtils::GetDefaultProfileOutputFile(appendString);
                }
                else if (config.bTrace || config.bHSATrace || config.bMergeMode)
                {
                    outputFileName = FileUtils::GetDefaultTraceOutputFile();
                }
                else if (config.bSubKernelProfile)
                {
                    outputFileName = FileUtils::GetDefaultSubKernelProfileOutputFile();
                }
                else if (config.bThreadTrace)
                {
                    outputFileName = FileUtils::GetDefaultThreadTraceOutputDir();
                }
            }
            else
            {
                config.strOutputFile = FileUtils::ToAbsPath(config.strOutputFile);

                size_t extensionPosition = config.strOutputFile.find_last_of('.');

                if (extensionPosition != std::string::npos)
                {
                    //Remove the extension
                    config.strOutputFile = config.strOutputFile.substr(0, extensionPosition);
                }

                outputFileName = config.strOutputFile;
                defaultOutputFileName = outputFileName;

                if (isCounterFileMoreThanOne)
                {
                    std::stringstream stringStream;
                    stringStream << "_pass" << (i + 1);
                    outputFileName += stringStream.str();
                }

                std::string strRequiredExt("");

                if (config.bPerfCounter || config.bHSAPMC)
                {
                    strRequiredExt.assign(PERF_COUNTER_EXT);
                    outputFileName = GetExpectedOutputFile(outputFileName, strRequiredExt);
                }
                else if (config.bTrace || config.bHSATrace || config.bMergeMode)
                {
                    strRequiredExt.assign(TRACE_EXT);
                    outputFileName = GetExpectedOutputFile(outputFileName, strRequiredExt);
                }
                else if (config.bOccupancy)
                {
                    strRequiredExt.assign(OCCUPANCY_EXT);
                    outputFileName = GetExpectedOutputFile(outputFileName, strRequiredExt);
                }
            }

            config.strOutputFile = outputFileName;
            outputFileList.push_back(outputFileName);
            retVal = ProcessCommandLine(config.counterFileList[i]);
            config.strOutputFile = passedOutputFileName;
            isReplaying = true;
        }
    }
    else
    {
        std::string counterFile;
        std::string outputFileName;

        if (config.strOutputFile.empty())
        {
            if (config.bTrace || config.bHSATrace || config.bMergeMode)
            {
                outputFileName = FileUtils::GetDefaultTraceOutputFile();
            }
            else if (config.bSubKernelProfile)
            {
                outputFileName = FileUtils::GetDefaultSubKernelProfileOutputFile();
            }
            else if (config.bThreadTrace)
            {
                outputFileName = FileUtils::GetDefaultThreadTraceOutputDir();
            }
            else if (config.bPerfCounter || config.bHSAPMC)
            {
                outputFileName = FileUtils::GetDefaultProfileOutputFile();
            }
            else if (config.bOccupancy)
            {
                outputFileName = FileUtils::GetDefaultOccupancyOutputFile();
            }
        }
        else
        {
            config.strOutputFile = FileUtils::ToAbsPath(config.strOutputFile);
            outputFileName = config.strOutputFile;

            std::string strRequiredExt("");

            if (config.bTrace || config.bHSATrace || config.bMergeMode)
            {
                strRequiredExt.assign(TRACE_EXT);
                outputFileName = GetExpectedOutputFile(outputFileName, strRequiredExt);
            }
            else if (config.bPerfCounter || config.bHSAPMC)
            {
                strRequiredExt.assign(PERF_COUNTER_EXT);
                outputFileName = GetExpectedOutputFile(outputFileName, strRequiredExt);
            }
            else if (config.bOccupancy)
            {
                strRequiredExt.assign(OCCUPANCY_EXT);
                outputFileName = GetExpectedOutputFile(outputFileName, strRequiredExt);
            }
        }

        if (config.counterFileList.size() == 1)
        {
            counterFile = config.counterFileList[0];
        }

        config.strOutputFile = outputFileName;
        outputFileList.push_back(outputFileName);
        retVal = ProcessCommandLine(counterFile);
    }

    if ((config.bHSAPMC || config.bPerfCounter) && outputFileList.size() > 1 && config.counterFileList.size() > 1)
    {
        GPA_API_Type apiName = GPA_API__LAST;

        if (config.bHSAPMC)
        {
            apiName = GPA_API_ROCM;
        }

        if (config.bPerfCounter)
        {
            apiName = GPA_API_OPENCL;
        }

        if (!MergeKernelProfileOutputFiles(config.counterFileList, outputFileList, defaultOutputFileName, apiName, config.bHSAPMC ? false : config.bGPUTimePMC))
        {
            retVal = -1;
        }
        else
        {
            retVal = 0;
        }
    }

    return retVal;
}

static void MergeFragFiles(int sig)
{
    if (!bDoneMerging)
    {
        MergeTraceFile(sig);
        MergeOccupancyFile(sig);
        bDoneMerging = true;
        signal(SIGINT, SIG_DFL);
    }
}

static void MergeTraceFile(int sig)
{
    SP_UNREFERENCED_PARAMETER(sig);

    if ((config.bTrace || config.bHSATrace || config.bMergeMode) && processId > 0)
    {
        std::string pid = StringUtils::ToString(processId);
        AtpFileWriter writer(config, pid);

        CLAtpFilePart* pOclTrace = nullptr;
        StackTraceAtpFilePart* pClStackTrace = nullptr;
        StackTraceAtpFilePart* pHsaStackTrace = nullptr;
        HSAAtpFilePart* pHsaTrace = nullptr;

        if (config.bTrace)
        {
            pOclTrace = new(std::nothrow) CLAtpFilePart(config);

            if (nullptr != pOclTrace)
            {
                writer.AddAtpFilePart(pOclTrace);
            }

            if (config.bSym)
            {
                pClStackTrace = new(std::nothrow) StackTraceAtpFilePart(std::string("ocl"), config);

                if (nullptr != pClStackTrace)
                {
                    writer.AddAtpFilePart(pClStackTrace);
                }
            }
        }

        if (config.bHSATrace)
        {
            pHsaTrace = new(std::nothrow) HSAAtpFilePart(config);

            if (nullptr != pHsaTrace)
            {
                writer.AddAtpFilePart(pHsaTrace);
            }

            if (config.bSym)
            {
                pHsaStackTrace = new(std::nothrow) StackTraceAtpFilePart(std::string("hsa"), config);

                if (nullptr != pHsaStackTrace)
                {
                    writer.AddAtpFilePart(pHsaStackTrace);
                }
            }
        }

        PerfMarkerAtpFilePart perfMarker(config);
        writer.AddAtpFilePart(&perfMarker);

        writer.SaveToAtpFile();

        SAFE_DELETE(pOclTrace);
        SAFE_DELETE(pClStackTrace);
        SAFE_DELETE(pHsaStackTrace);
        SAFE_DELETE(pHsaTrace);
    }
}

static bool SetHSAServer(const gtString& strDirPathUnicode)
{
    SP_TODO("Solve API Loader Issues for installed libraries in unicode directory");
    std::wstring strDirPathWide = strDirPathUnicode.asCharArray();
    std::string strDirPath;
    std::string rocpToolLibPath;
    StringUtils::WideStringToUtf8String(strDirPathWide, strDirPath);

    std::string strServerPath;

    if (config.bHSATrace)
    {
        strServerPath = strDirPath + HSA_TRACE_AGENT_DLL;
        rocpToolLibPath = strServerPath;

        if (config.bOccupancy)
        {
            std::cout << "Collecting Occupancy data is not supported when performing an Application Trace.  Use Performance Counter mode if you want to collect Occupancy data.\n";
        }
    }
    else if (config.bHSAPMC)
    {
        strServerPath = strDirPath + HSA_PROFILE_AGENT_DLL;
        rocpToolLibPath = strServerPath;
    }
    else
    {
        return false;
    }

    if (!FileUtils::FileExist(strServerPath))
    {
        size_t nSlashPos = strServerPath.find_last_of("/\\", strServerPath.size()) + 1;

        if (nSlashPos != std::string::npos)
        {
            strServerPath = strServerPath.substr(nSlashPos);
        }

        std::cout << strServerPath << " is missing" << std::endl;
        std::cout << "Make sure you have " << strServerPath << " under " << strDirPath.c_str() << std::endl;

        return false;
    }

    if (config.bHSAPMC || config.bHSATrace)
    {
        OSUtils::Instance()->SetEnvVar(ROCP_TOOL_LIB_ENV_VAR_NAME, rocpToolLibPath.c_str());
        std::string rocProfilerLib = "librocprofiler64.so";
        rocProfilerLib.append(" ");
        strServerPath = rocProfilerLib + strServerPath;
    }

    OSUtils::Instance()->SetEnvVar(HSA_ENABLE_PROFILING_ENV_VAR, strServerPath.c_str());

    if (config.bHSATrace)
    {
        OSUtils::Instance()->SetEnvVar(ROCP_TIMESTAMP_ON_ENV_VAR_NAME, "1");
    }

    return true;
}

static bool SetAgent(const gtString& strDirPathUnicode)
{
    SP_TODO("Solve API Loader Issues for installed libraries in unicode directory");
    std::wstring strDirPathWide = strDirPathUnicode.asCharArray();
    std::string strDirPath;
    StringUtils::WideStringToUtf8String(strDirPathWide, strDirPath);

    bool retVal = false;

    std::string strAgentDllPath;
    std::vector<std::string> vAgentDllPath;
    char szAgentDllPath[ SP_MAX_PATH ] = "\0";

    //   const unsigned int AGENT_OCC   = 4;

    if (config.bTrace)
    {
        SP_strcpy(szAgentDllPath, SP_MAX_PATH, CL_TRACE_AGENT_DLL);
        strAgentDllPath = strDirPath + std::string(szAgentDllPath);
        vAgentDllPath.push_back(strAgentDllPath);

        if (config.bOccupancy)
        {
            strAgentDllPath.clear();
            SP_strcpy(szAgentDllPath, SP_MAX_PATH, CL_OCCUPANCY_AGENT_DLL);
            strAgentDllPath = strDirPath + std::string(szAgentDllPath);
            vAgentDllPath.push_back(strAgentDllPath);
        }

        if (config.bSubKernelProfile)
        {
            strAgentDllPath.clear();
            SP_strcpy(szAgentDllPath, SP_MAX_PATH, CL_SUB_KERNEL_PROFILE_AGENT_DLL);
            strAgentDllPath = strDirPath + std::string(szAgentDllPath);
            vAgentDllPath.push_back(strAgentDllPath);
        }

        if (config.bPerfCounter)
        {
            strAgentDllPath.clear();
            SP_strcpy(szAgentDllPath, SP_MAX_PATH, CL_PROFILE_AGENT_DLL);
            strAgentDllPath = strDirPath + std::string(szAgentDllPath);
            vAgentDllPath.push_back(strAgentDllPath);
        }
    }
    else if (config.bSubKernelProfile)
    {

        if (config.bOccupancy)
        {
            strAgentDllPath.clear();
            SP_strcpy(szAgentDllPath, SP_MAX_PATH, CL_OCCUPANCY_AGENT_DLL);
            strAgentDllPath = strDirPath + std::string(szAgentDllPath);
            vAgentDllPath.push_back(strAgentDllPath);
        }

        strAgentDllPath.clear();
        SP_strcpy(szAgentDllPath, SP_MAX_PATH, CL_SUB_KERNEL_PROFILE_AGENT_DLL);
        strAgentDllPath = strDirPath + std::string(szAgentDllPath);
        vAgentDllPath.push_back(strAgentDllPath);

        if (config.bPerfCounter)
        {
            strAgentDllPath.clear();
            SP_strcpy(szAgentDllPath, SP_MAX_PATH, CL_PROFILE_AGENT_DLL);
            strAgentDllPath = strDirPath + std::string(szAgentDllPath);
            vAgentDllPath.push_back(strAgentDllPath);
        }
    }
    else if (config.bPerfCounter)
    {
        if (config.bOccupancy)
        {
            strAgentDllPath.clear();
            SP_strcpy(szAgentDllPath, SP_MAX_PATH, CL_OCCUPANCY_AGENT_DLL);
            strAgentDllPath = strDirPath + std::string(szAgentDllPath);
            vAgentDllPath.push_back(strAgentDllPath);
        }

        strAgentDllPath.clear();
        SP_strcpy(szAgentDllPath, SP_MAX_PATH, CL_PROFILE_AGENT_DLL);
        strAgentDllPath = strDirPath + std::string(szAgentDllPath);
        vAgentDllPath.push_back(strAgentDllPath);
    }
    else if (config.bOccupancy && !config.bHSAPMC  && !config.bHSATrace)
    {
        strAgentDllPath.clear();
        SP_strcpy(szAgentDllPath, SP_MAX_PATH, CL_OCCUPANCY_AGENT_DLL);
        strAgentDllPath = strDirPath + std::string(szAgentDllPath);
        vAgentDllPath.push_back(strAgentDllPath);
    }
    else if (config.bThreadTrace)
    {
        strAgentDllPath.clear();
        SP_strcpy(szAgentDllPath, SP_MAX_PATH, CL_THREAD_TRACE_AGENT_DLL);
        strAgentDllPath = strDirPath + std::string(szAgentDllPath);
        vAgentDllPath.push_back(strAgentDllPath);
    }

    for (unsigned int i = 0; i < vAgentDllPath.size(); ++i)
    {
        if (!FileUtils::FileExist(vAgentDllPath[i]))
        {
            std::string strAgent = vAgentDllPath[i];
            size_t nSlashPos = strAgent.find_last_of("/\\", strAgent.size()) + 1;

            if (nSlashPos != std::string::npos)
            {
                strAgent = strAgent.substr(nSlashPos);
            }

            std::cout << strAgent << " is missing" << std::endl;
            std::cout << "Make sure you have " << strAgent << " under " << strDirPath.c_str() << std::endl;

            for (unsigned int j = i; j < vAgentDllPath.size() - 1; ++j)
            {
                vAgentDllPath[j] = vAgentDllPath[j + 1];
            }

            vAgentDllPath.pop_back();
            i--;
        }
    }

    // Check existing agents
    std::string agents = OSUtils::Instance()->GetEnvVar(OCL_ENABLE_PROFILING_ENV_VAR);

    if (!agents.empty())
    {
        // Each token should be ended by a comma to signal the end of the token.
        // If the agents string does not end with a comma, append it

        size_t nAgentStringLength = agents.length();

        if (agents[nAgentStringLength] != ',')
        {
            agents.append(",");
        }

        //If the agents are not empty, parse the values of the environment variable
        std::string strToken;
        size_t nPos1 = 0;
        size_t nPos2 = agents.find_first_of(",", nPos1);

        while (nPos2 != std::string::npos)
        {
            strToken = agents.substr(nPos1, nPos2);

            // get ready to extract the next token
            nPos1 = nPos2;
            nPos2 = agents.find_first_of(",", nPos1 + 1);

            // After having extracted the agent strings, check if it agent has already been set
            // via the switches
            if (strToken.find(CL_AGENT_TRACE) != std::string::npos)
            {
                if (!config.bTrace)
                {
                    vAgentDllPath.insert(vAgentDllPath.begin(), strToken);
                }
            }
            else if (strToken.find(CL_AGENT_OCCUPANCY) != std::string::npos)
            {
                if (!config.bOccupancy)
                {
                    if (vAgentDllPath[0].find(CL_AGENT_TRACE) != std::string::npos)
                    {
                        vAgentDllPath.insert(vAgentDllPath.begin() + 1, strToken);
                    }
                    else
                    {
                        vAgentDllPath.insert(vAgentDllPath.begin(), strToken);
                    }
                }
            }
            else if (strToken.find(CL_AGENT_SUB_KRNL) != std::string::npos)
            {
                if (!config.bSubKernelProfile)
                {
                    if (vAgentDllPath[ vAgentDllPath.size() - 1 ].find(CL_AGENT_PERF_CTR) != std::string::npos)
                    {
                        vAgentDllPath.insert(vAgentDllPath.begin() + vAgentDllPath.size() - 2, strToken);
                    }
                    else
                    {
                        vAgentDllPath.push_back(strToken);
                    }
                }
            }
            else if (strToken.find(CL_AGENT_PERF_CTR) != std::string::npos)
            {
                if (!config.bPerfCounter)
                {
                    vAgentDllPath.push_back(strToken);
                }
            }
            else //not a recognized token
            {
                if (!strToken.empty())   //assume a user-specified token
                {
                    if (vAgentDllPath[0].find(CL_AGENT_TRACE) != std::string::npos)
                    {
                        if (vAgentDllPath.size() > 1)
                        {
                            // Additional token, so need to check if it is occupancy agent or not
                            // If occupancy agent, append after occupancy agent, otherwise, before next
                            // agent
                            if (vAgentDllPath[1].find(CL_AGENT_OCCUPANCY) != std::string::npos)
                            {
                                vAgentDllPath.insert(vAgentDllPath.begin() + 2, strToken);
                            }
                            else
                            {
                                vAgentDllPath.insert(vAgentDllPath.begin() + 1, strToken);
                            }
                        }
                        else
                        {
                            // At end of agent list, so add user-token to end
                            vAgentDllPath.push_back(strToken);
                        }
                    }
                    else
                    {
                        if (vAgentDllPath[0].find(CL_AGENT_OCCUPANCY) != std::string::npos)
                        {
                            vAgentDllPath.insert(vAgentDllPath.begin() + 1, strToken);
                        }
                        else
                        {
                            vAgentDllPath.insert(vAgentDllPath.begin(), strToken);
                        }
                    }
                }
            }
        }


        //concatenate the agent dlls into a string (comma-separated and terminated)
        //    agents = szSecondAgentDllPath + ',' + agents + ',';
        agents.clear();

        for (int i = (int)vAgentDllPath.size() - 1; i >= 0; i--)
        {
            agents += vAgentDllPath[i] + ",";
        }
    }
    else
    {
        agents.clear();

        for (int i = (int)vAgentDllPath.size() - 1; i >= 0; i--)
        {
            agents += vAgentDllPath[i] + ",";
        }
    }

    if (!agents.empty())
    {
        // Set environment variable
        OSUtils::Instance()->SetEnvVar(OCL_ENABLE_PROFILING_ENV_VAR, agents.c_str());
        retVal = true;
    }

    return retVal;
}

#if defined (_LINUX) || defined (LINUX)

static bool SetPreLoadLibs()
{
    bool retVal = true;

    if (!config.strPreloadLib.empty())
    {
        std::string curPreLoadPath = OSUtils::Instance()->GetEnvVar(LD_PRELOAD_ENV_VAR_NAME);

        if (!curPreLoadPath.empty())
        {
            curPreLoadPath = ":" + curPreLoadPath;
        }

        curPreLoadPath = config.strPreloadLib + curPreLoadPath;

        retVal = OSUtils::Instance()->SetEnvVar(LD_PRELOAD_ENV_VAR_NAME, curPreLoadPath.c_str());
    }

    return retVal;
}

#endif

EnvSysBlockString GetEnvironmentBlock(EnvVarMap mapUserBlock, bool bIncludeSystemEnv)
{
    EnvVarMap envBlock;

    if (bIncludeSystemEnv)
    {
        // first, get the entire environment block for the process
        ENVSYSBLOCK envStrs = OSUtils::Instance()->GetSysEnvBlock();

        if (envStrs != NULL)
        {
            ENVSYSBLOCK pCurEnvVar = envStrs;

            EnvSysBlockString strCurEnvVar;
            EnvSysBlockString varName;
            EnvSysBlockString varValue;

            while (*pCurEnvVar != '\0')
            {
                strCurEnvVar = pCurEnvVar;

                size_t equalPos = strCurEnvVar.find('=');

                if (equalPos != 0)
                {
                    if (equalPos != std::string::npos)
                    {
                        varName = strCurEnvVar.substr(0, equalPos);
                        varValue = strCurEnvVar.substr(equalPos + 1);
                    }
                    else
                    {
                        varName = strCurEnvVar;
                        varValue.clear();
                    }

#ifdef _WIN32
                    gtString strVarName(varName.c_str());
                    gtString strVarValue(varValue.c_str());
#elif defined (_LINUX) || defined (LINUX)
                    gtString strVarName;
                    gtString strVarValue;
                    strVarName.fromASCIIString(varName.c_str());
                    strVarValue.fromASCIIString(varValue.c_str());
#endif

                    envBlock[strVarName] = strVarValue;
                }

                pCurEnvVar += strCurEnvVar.length() + 1;
            }

            OSUtils::Instance()->ReleaseSysEnvBlock(envStrs);
        }
    }

#if defined (_LINUX) || defined (LINUX)
    std::string strAgentVar = OSUtils::Instance()->GetEnvVar(OCL_ENABLE_PROFILING_ENV_VAR);

    if (!strAgentVar.empty())
    {
        gtString agentVar;
        gtString agentVarVal;
        agentVar.fromASCIIString(OCL_ENABLE_PROFILING_ENV_VAR);
        agentVarVal.fromASCIIString(strAgentVar.c_str());
        envBlock[agentVar] = agentVarVal;
    }

    strAgentVar = OSUtils::Instance()->GetEnvVar(HSA_ENABLE_PROFILING_ENV_VAR);

    if (!strAgentVar.empty())
    {
        gtString agentVar;
        gtString agentVarVal;
        agentVar.fromASCIIString(HSA_ENABLE_PROFILING_ENV_VAR);
        agentVarVal.fromASCIIString(strAgentVar.c_str());
        envBlock[agentVar] = agentVarVal;
    }

    strAgentVar = OSUtils::Instance()->GetEnvVar(LD_PRELOAD_ENV_VAR_NAME);

    if (!strAgentVar.empty())
    {
        gtString agentVar;
        gtString agentVarVal;
        agentVar.fromASCIIString(LD_PRELOAD_ENV_VAR_NAME);
        agentVarVal.fromASCIIString(strAgentVar.c_str());
        envBlock[agentVar] = agentVarVal;
    }

    strAgentVar = OSUtils::Instance()->GetEnvVar(ROCP_HSA_INTERCEPT_ENV_VAR_NAME);

    if (!strAgentVar.empty())
    {
        gtString agentVar;
        gtString agentVarVal;
        agentVar.fromASCIIString(ROCP_HSA_INTERCEPT_ENV_VAR_NAME);
        agentVarVal.fromASCIIString(strAgentVar.c_str());
        envBlock[agentVar] = agentVarVal;
    }

    strAgentVar = OSUtils::Instance()->GetEnvVar(ROCP_TOOL_LIB_ENV_VAR_NAME);

    if (!strAgentVar.empty())
    {
        gtString agentVar;
        gtString agentVarVal;
        agentVar.fromASCIIString(ROCP_TOOL_LIB_ENV_VAR_NAME);
        agentVarVal.fromASCIIString(strAgentVar.c_str());
        envBlock[agentVar] = agentVarVal;
    }

    strAgentVar = OSUtils::Instance()->GetEnvVar(ROCP_TIMESTAMP_ON_ENV_VAR_NAME);

    if (!strAgentVar.empty())
    {
        gtString agentVar;
        gtString agentVarVal;
        agentVar.fromASCIIString(ROCP_TIMESTAMP_ON_ENV_VAR_NAME);
        agentVarVal.fromASCIIString(strAgentVar.c_str());
        envBlock[agentVar] = agentVarVal;
    }
#endif

    // next, replace or add the user-specified env vars
    for (EnvVarMap::const_iterator it = mapUserBlock.begin(); it != mapUserBlock.end(); it++)
    {
        envBlock[(*it).first] = (*it).second;
    }

    // finally, build a zero-separated, double-zero-terminated string that can be passed to CreateProcess
    EnvSysBlockString strEnvVarBlock;

    for (EnvVarMap::const_iterator it = envBlock.begin(); it != envBlock.end(); it++)
    {
#ifdef _WIN32
        EnvSysBlockString strValName((*it).first.asCharArray());
        EnvSysBlockString strValValue((*it).second.asCharArray());
#elif defined (_LINUX) || defined (LINUX)
        EnvSysBlockString strValName((*it).first.asASCIICharArray());
        EnvSysBlockString strValValue((*it).second.asASCIICharArray());
#endif
        strEnvVarBlock += strValName + ENVVARSEPARATOR + strValValue + ENVBLOCKDELIMITER;
    }

    strEnvVarBlock += ENVBLOCKDELIMITER;

    return strEnvVarBlock;
}

bool CheckIsAppValid(const gtString& strAppName, int iProfilerNbrBits)
{
    bool bStatus = true;


    int nFileType = FileUtils::GetBinaryFileType(strAppName);

    if (nFileType < 0)
    {
        std::cout << "Unable to open the application to be profiled to determine file type" << std::endl;
    }

    if (nFileType == FileUtils::FTYPE_EXE)
    {
        //Assume that the app to be profiled is a valid executable, next, we check that the bitness
        //is the same as that of the profiler

        int iBinaryFileNbrBits = FileUtils::GetBinaryNbrBits(strAppName);

        if (iBinaryFileNbrBits != iProfilerNbrBits)
        {
            std::cout << "The profiler is a " << iProfilerNbrBits << " bit application while the application to be profiled is " << iBinaryFileNbrBits << " bits." << std::endl;
            std::cout << "The number of address bits of the profiler must match the number of the address bits of the application being profiled" << std::endl;
            bStatus = false;
        }
    }
    else
    {
#if defined (_LINUX) || defined (LINUX)
        std::string convertedName;
        StringUtils::WideStringToUtf8String(strAppName.asCharArray(), convertedName);

        struct stat statBuf = {};
        bStatus = (stat(convertedName.c_str(), &statBuf) == 0) && (statBuf.st_mode & S_IXUSR);
#else
        bStatus = false;
#endif
    }


    return bStatus;
}

int GetNbrAppBits(const gtString& strProfiler)
{
    int nBits = FileUtils::FILE_BITS_UNKNOWN;
    nBits = FileUtils::GetBinaryNbrBits(strProfiler);
    return nBits;
}

static void MergeOccupancyFile(int sig)
{
    SP_UNREFERENCED_PARAMETER(sig);

    if (((config.bOccupancy && config.bTimeOut) || config.bMergeMode) && processId > 0)
    {
        CLOccupancyHdr header;
        header.m_iVersionMajor = RCP_MAJOR_VERSION;
        header.m_iVersionMinor = RCP_MINOR_VERSION;
        header.m_strAppName = config.strInjectedApp;
        header.m_strAppArgs = config.strInjectedAppArgs;
        header.m_listSeparator = config.cOutputSeparator;
        std::string pid = StringUtils::ToString(processId);

        std::string occupancyFile = config.strOutputFile;

        if ((config.bHSAPMC || config.bPerfCounter) && config.counterFileList.size() > 1)
        {
            size_t passStringPosition = config.strOutputFile.find("_pass");

            if (passStringPosition != std::string::npos)
            {
                //Remove the appended "_pass"" string and the extension
                occupancyFile = config.strOutputFile.substr(0, passStringPosition);
            }
        }

        MergeTmpCLOccupancyFile(occupancyFile, strTmpFilePath, pid, header);
    }

    signal(SIGINT, SIG_DFL);
}
