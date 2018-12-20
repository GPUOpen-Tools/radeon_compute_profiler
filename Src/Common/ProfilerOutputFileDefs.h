//==============================================================================
// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief This file contains Profiler Output files string constants
//==============================================================================

#ifndef _PROFILER_OUTPUT_FILE_DEFS_H_
#define _PROFILER_OUTPUT_FILE_DEFS_H_

// CodeXL string
#define CODEXL_STR "CodeXL "

// Atp Section Part Names
#define CL_PART_NAME "ocl"
#define HSA_PART_NAME "hsa"
#define PERFMARKER_PART_NAME "perfmarker"

// Profiler Output File Extensions
#define TMP_TIME_STAMP_EXT ".tstamp"
#define TMP_GPU_TIME_STAMP_RAW_EXT ".ocltstampraw"
#define TMP_KERNEL_TIME_STAMP_EXT ".kerneltstamp"
#define TMP_ASYNC_COPY_TIME_STAMP_EXT ".copytstamp"
#define TMP_TRACE_EXT ".apitrace"
#define TMP_TRACE_STACK_EXT ".stfrag"
#define TMP_OCCUPANCY_EXT ".occupancyfrag"
#define TRACE_STACK_EXT ".st"
#define PERFMARKER_EXT ".amdtperfmarker"
#define OCCUPANCY_EXT "occupancy"
#define TRACE_EXT "atp"
#define PERF_COUNTER_EXT "csv"
#define DEFAULT_OUTPUT_FILE "session1"
#define KERNEL_ASSEMBLY_FILE_PREFIX "sp_tmp."

// Common File Defs
#define FILE_HEADER_DEFAULT_COMMENT_CHAR '#';
#define FILE_HEADER_PROFILER_VERSION "ProfilerVersion"
#define FILE_HEADER_APPLICATION "Application"
#define FILE_HEADER_APPLICATION_ARGS "ApplicationArgs"
#define FILE_HEADER_WORKING_DIRECTORY "WorkingDirectory"
#define FILE_HEADER_FULL_ENVIRONMENT "FullEnvironment"
#define FILE_HEADER_ENV_VAR "EnvVar"
#define FILE_HEADER_USER_TIMER "UserTimer"
#define FILE_HEADER_OS_VERSION "OS Version"
#define FILE_HEADER_DISPLAY_NAME "DisplayName"
#define FILE_HEADER_API "API"
#define FILE_HEADER_LIST_SEPARATOR "ListSeparator"
#define FILE_HEADER_TRACE_FILE_VERSION "TraceFileVersion"
#define FILE_HEADER_EXCLUDED_APIS "ExcludedAPIs"
#define FILE_HEADER_PROFILE_FILE_VERSION "ProfileFileVersion"
#define FILE_HEADER_FORCE_SINGLE_PASS "ForceSinglePass"
#define FILE_HEADER_MAX_NUMBER_OF_KERNELS_TO_PROFILE "MaxNumberOfKernelsToProfile"
#define FILE_HEADER_KERNELS_PROFILED "KernelsProfiled"
#define FILE_HEADER_KERNEL_COUNT "KernelCount"

// ATP File
#define ATP_API_TRACE_OUTPUT " API Trace Output"
#define ATP_TIMESSTAMP_OUTPUT " Timestamp Output"
#define ATP_KERNEL_TIMESTAMP_OUTPUT " Kernel Timestamp Output"
#define ATP_STACK_TRACE_OUTPUT " Stack Trace Output"
#define ATP_SECTION_HEADER_START_END "====="

// Occupancy File Defs
#define OCCUPANCY_DEFAULT_LIST_SEPARATOR_CHAR ','
#define OCCUPANCY_COLUMN_NAME_THREADID "Thread ID"
#define OCCUPANCY_COLUMN_NAME_KERNELNAME "Kernel Name"
#define OCCUPANCY_COLUMN_NAME_DEVICENAME "Device Name"
#define OCCUPANCY_COLUMN_NAME_NUMBEROFCOMPUTEUNITS "Number of compute units"
#define OCCUPANCY_COLUMN_NAME_MAXNUMBEROFWAVEFRONTSPERCU "Max. number of wavefronts per CU"
#define OCCUPANCY_COLUMN_NAME_MAXNUMBEROFWORKGROUPPERCU "Max. number of work-group per CU"
#define OCCUPANCY_COLUMN_NAME_MAXNUMBEROFVGPR "Max. number of VGPR"
#define OCCUPANCY_COLUMN_NAME_MAXNUMBEROFSGPR "Max. number of SGPR"
#define OCCUPANCY_COLUMN_NAME_MAXAMOUNTOFLDS "Max. amount of LDS"
#define OCCUPANCY_COLUMN_NAME_NUMBEROFVGPRUSED "Number of VGPR used"
#define OCCUPANCY_COLUMN_NAME_NUMBEROFSGPRUSED "Number of SGPR used"
#define OCCUPANCY_COLUMN_NAME_AMOUNTOFLDSUSED "Amount of LDS used"
#define OCCUPANCY_COLUMN_NAME_SIZEOFWAVEFRONT "Size of wavefront"
#define OCCUPANCY_COLUMN_NAME_WORKGROUPSIZE "Work-group size"
#define OCCUPANCY_COLUMN_NAME_WAVEFRONTSPERWORKGROUP "Wavefronts per work-group"
#define OCCUPANCY_COLUMN_NAME_MAXWORKGROUPSIZE "Max work-group size"
#define OCCUPANCY_COLUMN_NAME_MAXWAVEFRONTSPERWORKGROUP "Max wavefronts per work-group"
#define OCCUPANCY_COLUMN_NAME_GLOBALWORKSIZE "Global work size"
#define OCCUPANCY_COLUMN_NAME_MAXIMUMGLOBALWORKSIZE "Maximum global work size"
#define OCCUPANCY_COLUMN_NAME_NBRVGPRLIMITEDWAVES "Nbr VGPR-limited waves"
#define OCCUPANCY_COLUMN_NAME_NBRSGPRLIMITEDWAVES "Nbr SGPR-limited waves"
#define OCCUPANCY_COLUMN_NAME_NBRLDSLIMITEDWAVES "Nbr LDS-limited waves"
#define OCCUPANCY_COLUMN_NAME_NBROFWGLIMITEDWAVES "Nbr of WG-limited waves"
#define OCCUPANCY_COLUMN_NAME_KERNELOCCUPANCY "Kernel occupancy"
#define OCCUPANCY_COLUMN_NAME_GRAPHICSIPVERSION "Graphics IP Version"
#define OCCUPANCY_COLUMN_NAME_SIMDSPERCU "Number of SIMDs per CU"

// Perf Marker Defs
#define PERF_MARKER_CL_BEGIN_PERFMARKER "clBeginPerfMarker"
#define PERF_MARKER_CL_END_PERFMARKER "clEndPerfMarker"
#define PERF_MARKER_CL_END_EX_PERFMARKER "clEndPerfMarkerEx"
#define ATP_PERFMARKER_SECTION_NAME "Perfmarker Output"
#define ATP_PERFMARKER_SECTION_NAME_PREV "OpenCL Perfmarker Output"

// CSV File Defs
#define CSV_DEFAULT_LIST_SEPARATOR_CHAR OCCUPANCY_DEFAULT_LIST_SEPARATOR_CHAR
#define CSV_COMMON_COLUMN_METHOD "Method"
#define CSV_COMMON_COLUMN_EXECUTION_ORDER "ExecutionOrder"
#define CSV_COMMON_COLUMN_THREAD_ID "ThreadID"
#define CSV_COMMON_COLUMN_CALL_INDEX "CallIndex"
#define CSV_COMMON_COLUMN_GLOBAL_WORK_SIZE "GlobalWorkSize"
#define CSV_COMMON_COLUMN_WORK_GROUP_SIZE "WorkGroupSize"
#define CSV_COMMON_COLUMN_TIME "Time"
#define CSV_COMMON_COLUMN_LOCAL_MEM_SIZE "LocalMemSize"
#define CSV_COMMON_COLUMN_VGPRs "VGPRs"
#define CSV_COMMON_COLUMN_SGPRs "SGPRs"
#define CSV_COMMON_COLUMN_SCRATCH_REGS "ScratchRegs"

#define EQUAL_SIGN_STR "="
#define SEMI_COLON_STR ";"
#define HASH_SIGN_WITH_SPACE_STR "# "

#define ATP_PARAM_VALUE_DELIMITER EQUAL_SIGN_STR
#define ATP_TRACE_ENTRY_ARG_SEPARATOR SEMI_COLON_STR
#define ATP_TRACE_STRUCT_ARG_SEPARATOR ","
#define OCCUPANCY_HEADER_START HASH_SIGN_WITH_SPACE_STR

#endif // _PROFILER_OUTPUT_FILE_DEFS_H_
