//==============================================================================
// Copyright (c) 2015-2018 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Some common definitions.
//==============================================================================

#ifndef _DEFS_H_
#define _DEFS_H_

// std
#include <string>
#include <cstring>
#include <map>
#include <unordered_set>
#include <assert.h>

// common
#include <AMDTBaseTools/Include/gtString.h>

#include "Version.h"

/// \defgroup Common Common
/// This module consists of the common functionalities used by other modules.
///
/// \ingroup Backend
// @{

#ifdef WIN32
    #if AMDT_ADDRESS_SPACE_TYPE == AMDT_64_BIT_ADDRESS_SPACE
        #define BITNESS "-x64"
        #define NUMBITS "64"
    #else
        #define BITNESS ""
        #define NUMBITS "32"
    #endif

    #define LIB_PREFIX ""
    #define LIB_SUFFIX ".dll"
    #define EXE_SUFFIX ".exe"
#else
    #ifdef X86
        #define BITNESS "32"
        #define NUMBITS "32"
    #else
        #define BITNESS ""
        #define NUMBITS "64"
    #endif

    #define LIB_PREFIX "lib"
    #define LIB_SUFFIX ".so"
    #define EXE_SUFFIX ""
#endif

#define CL_TRACE_AGENT_DLL LIB_PREFIX GPU_PROFILER_LIBRARY_NAME_PREFIX "CLTraceAgent" BITNESS AMDT_DEBUG_SUFFIX AMDT_BUILD_SUFFIX LIB_SUFFIX
#define CL_SUB_KERNEL_PROFILE_AGENT_DLL LIB_PREFIX GPU_PROFILER_LIBRARY_NAME_PREFIX "CLSubKernelProfileAgent" BITNESS AMDT_DEBUG_SUFFIX AMDT_BUILD_SUFFIX LIB_SUFFIX
#define CL_PROFILE_AGENT_DLL LIB_PREFIX GPU_PROFILER_LIBRARY_NAME_PREFIX "CLProfileAgent" BITNESS AMDT_DEBUG_SUFFIX AMDT_BUILD_SUFFIX LIB_SUFFIX
#define CL_OCCUPANCY_AGENT_DLL LIB_PREFIX GPU_PROFILER_LIBRARY_NAME_PREFIX "CLOccupancyAgent" BITNESS AMDT_DEBUG_SUFFIX AMDT_BUILD_SUFFIX LIB_SUFFIX
#define CL_THREAD_TRACE_AGENT_DLL LIB_PREFIX GPU_PROFILER_LIBRARY_NAME_PREFIX "CLThreadTraceAgent" BITNESS AMDT_DEBUG_SUFFIX AMDT_BUILD_SUFFIX LIB_SUFFIX

#define PROFILE_DATA_PARSER_DLL LIB_PREFIX GPU_PROFILER_LIBRARY_NAME_PREFIX "ProfileDataParser" BITNESS AMDT_DEBUG_SUFFIX AMDT_BUILD_SUFFIX LIB_SUFFIX

#define HSA_TRACE_AGENT_DLL LIB_PREFIX GPU_PROFILER_LIBRARY_NAME_PREFIX "HSATraceAgent" BITNESS AMDT_DEBUG_SUFFIX AMDT_BUILD_SUFFIX LIB_SUFFIX
#define HSA_PROFILE_AGENT_DLL LIB_PREFIX GPU_PROFILER_LIBRARY_NAME_PREFIX "HSAProfileAgent" BITNESS AMDT_DEBUG_SUFFIX AMDT_BUILD_SUFFIX LIB_SUFFIX

#define PROFILER_EXE "rcprof" BITNESS AMDT_DEBUG_SUFFIX AMDT_BUILD_SUFFIX EXE_SUFFIX

#ifdef WIN32
    #if AMDT_ADDRESS_SPACE_TYPE == AMDT_64_BIT_ADDRESS_SPACE
        #define HSA_RUNTIME_TOOLS_LIB "hsa-runtime-tools64.dll"
    #else
        #define HSA_RUNTIME_TOOLS_LIB "hsa-runtime-tools.dll"
    #endif
#else
    #ifdef X86
        #define HSA_RUNTIME_TOOLS_LIB "libhsa-runtime-tools.so.1"
    #else
        #define HSA_RUNTIME_TOOLS_LIB "libhsa-runtime-tools64.so.1"
    #endif
#endif

#ifdef WIN32
    #define MICRO_DLL LIB_PREFIX GPU_PROFILER_LIBRARY_NAME_PREFIX "MicroDll" BITNESS AMDT_DEBUG_SUFFIX AMDT_BUILD_SUFFIX LIB_SUFFIX
#elif defined (__linux__) || defined (_LINUX) || defined(LINUX)
    #define PRELOAD_XINITTHREADS_LIB LIB_PREFIX GPU_PROFILER_LIBRARY_NAME_PREFIX "PreloadXInitThreads" BITNESS AMDT_DEBUG_SUFFIX AMDT_BUILD_SUFFIX LIB_SUFFIX
#endif

#define HSA_ENABLE_PROFILING_ENV_VAR "HSA_TOOLS_LIB"
#define OCL_ENABLE_PROFILING_ENV_VAR "CL_AGENT"

#define STRLEN 100

#ifndef ULONGLONG
    typedef unsigned long long ULONGLONG;
#endif

#ifdef WIN32
    #define SP_MAX_PATH MAX_PATH
#else
    #define SP_MAX_PATH 4096
#endif

/// max number of command line arguments we support in Linux build
#define SP_MAX_ARG 128
/// max number of environment variables we support in Linux build
#define SP_MAX_ENVVARS 256

#define SP_MAX_ENVVAR_SIZE 8191 // max length for Windows, seems reasonably long for Linux

/// max number of properties to show when dereferencing the properties parameter for clCreateContext, clCreateContextFromType and clGetGLContextInfoKHR
#define SP_MAX_NUM_CONTEXT_PROPERTIES 64

#ifndef _DEBUG
    #define DISABLE_LOG
#endif

#ifndef SAFE_RELEASE
    #define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p)=NULL; } }
#endif

#ifndef SAFE_FREE
    #define SAFE_FREE(p) { if (p) { free(p); p=NULL; } }
#endif

#ifndef SAFE_DELETE
    #define SAFE_DELETE(p) { if (p) { delete p; p=NULL; } }
#endif

#ifndef SAFE_DELETE_ARRAY
    #define SAFE_DELETE_ARRAY(p) { if (p) { delete [] p; p=NULL; } }
#endif

#if defined (_WIN32)
    #define SP_UNREFERENCED_PARAMETER( x ) ( x )
#elif defined (__linux__) || defined (__CYGWIN__) || defined (_LINUX) || defined(LINUX)
    #define SP_UNREFERENCED_PARAMETER( x ) (void)(x)
#endif

#ifdef _WIN32
    //#define USE_TEXT_WRITER
#endif

#define hidden_quote( s ) #s
#define hidden_numquote( n ) hidden_quote( n )

#ifndef SUPPRESS_TODO_MSG
    #if defined (_WIN32)

        #define SP_TODO(x)  __pragma( message( __FILE__ "(" hidden_numquote( __LINE__ ) "): TODO: " x ) )

    #elif defined (__linux__) || defined (_LINUX) || defined(LINUX)

        // Macros do not seem to directly expand on Linux in #pragma statements
        #define DO_PRAGMA(x)    _Pragma(#x)
        #define SP_TODO(x)  DO_PRAGMA( message( __FILE__ "(" hidden_numquote( __LINE__ ) "): TODO: " x ) )

    #elif defined (__CYGWIN__)

        #define SP_TODO(x)

    #endif
#else
    #define SP_TODO(x)
#endif

#ifndef DLL_PUBLIC
    #if defined _WIN32 || defined __CYGWIN__
        #ifdef __GNUC__
            #define DLL_PUBLIC __attribute__ ((dllexport))
        #else
            #define DLL_PUBLIC __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
        #endif
        #ifndef DLL_LOCAL
            #define DLL_LOCAL
        #endif
    #else
        #if __GNUC__ >= 4
            #define DLL_PUBLIC __attribute__ ((visibility ("default")))
            #ifndef DLL_LOCAL
                #define DLL_LOCAL  __attribute__ ((visibility ("hidden")))
            #endif
        #else
            #define DLL_PUBLIC
            #ifndef DLL_LOCAL
                #define DLL_LOCAL
            #endif
        #endif
    #endif
#endif //DLL_PUBLIC

// HTML symbol names
#define SPACE "&nbsp;"
#define COMMA "&comma;"
#define AMPERSAND "&amp;"
#define NUMSIGN "&#35;"
#define LESSTHAN "&lt;"
#define GREATERTHAN "&gt;"
#define ATSYMBOL "&#64;"

#define DEFAULT_TIMEOUT_INTERVAL 100

#define DEFAULT_MAX_NUM_OF_API_CALLS 1000000
#define DEFAULT_MAX_KERNELS 100000

#define TEST_EXCEPTION_EXIT_CODE 0xBAD99999

#define CASE(x) case x: return #x;
#define CASESTR(x) case x: ss << #x; break;
#define ENUMTOSTRING(X) return #X;

// Call clGetEventInfo to retrieve ref count in Trace_func
// Create overhead, debug use only
//#define _DEBUG_REF_COUNT_

/// Map of the current environment variables
typedef std::map<gtString, gtString> EnvVarMap;

/// List of kernels to be profiled
typedef std::unordered_set<std::string> KernelFilterList;

/// Parameters passed from rcprof to servers


typedef std::map<std::string, bool> AnalyzerMap;

/// Templated Less<T> for comparing member variables
template <class Class, typename Type, Type Class::*PtrToMem>
struct MemberCmp
{
    bool operator()(const Class* left, const Class* right)
    {
        return left->*PtrToMem < right->*PtrToMem;
    }
};

/// Structure for analyze options
struct AnalyzeOps
{
    std::string strAtpFile;         ///< Input Atp file
    std::string strCsvFile;         ///< Input csv file
    bool bContextSummary;           ///< Indicating whether we generate summary page for context
    bool bTop10KernelSummary;       ///< Indicating whether we generate summary page for top 10 kernel
    bool bTop10DataTransferSummary; ///< Indicating whether we generate summary page for top 10 data transfer
    bool bDataTransferSummary;      ///< Indicating whether we generate summary page for data transfers
    bool bDataTransferList;         ///< Indicating whether we generate summary page for list of data transfers
    bool bAPISummary;               ///< Indicating whether we generate summary page for API
    bool bKernelSummary;            ///< Indicating whether we generate summary page for kernel summary
    bool bKernelList;               ///< Indicating whether we generate summary page for list of kernels
    bool bBestPractices;            ///< Indicating whether we generate best practices page
    AnalyzerMap analyzerMap;        ///< Analyzer map

    /// default constructor
    AnalyzeOps() :
        bContextSummary(true),
        bTop10KernelSummary(true),
        bTop10DataTransferSummary(true),
        bDataTransferSummary(true),
        bDataTransferList(true),
        bAPISummary(true),
        bKernelSummary(true),
        bKernelList(true),
        bBestPractices(true)
    {
        strAtpFile.clear();
        strCsvFile.clear();
    }
};

// @}

#endif // _DEFS_H_
