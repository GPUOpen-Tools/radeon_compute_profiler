//==============================================================================
// Copyright (c) 2015-2018 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief This file contains the function to analyze profile/trace output
//==============================================================================

#ifndef _ANALYZE_H_
#define _ANALYZE_H_

#include "Config.h"

#define CLAPI_SUM "CLAPISummary.html"
#define CLCTX_SUM "CLContextSummary.html"
#define CLTOP10_KERNEL "CLTop10KernelSummary.html"
#define CLLIST_KERNEL "CLKernelSummaryList.html"
#define CLKERNEL_SUM "CLKernelSummary.html"
#define CLTOP10_DATA "CLTop10DataTransferSummary.html"
#define CLDATA_TRANSFERS "CLDataTransferList.html"
#define CLDATA_SUM "CLDataTransferSummary.html"
#define CLBEST_PRACTICES "CLBestPractices.html"

#define HSAAPI_SUM "HSAAPISummary.html"
#define HSAKERNEL_SUM "HSAKernelSummary.html"
#define HSATOP10_KERNEL "HSATop10KernelSummary.html"
#define HSALIST_KERNEL "HSAKernelSummaryList.html"
#define HSATOP10_DATA "HSATop10DataTransferSummary.html"
#define HSADATA_TRANSFERS "HSADataTransferList.html"
#define HSADATA_SUM "HSADataTransferSummary.html"
#define HSABEST_PRACTICES "HSABestPractices.html"

bool APITraceAnalyze(const Config& config);

#endif //_ANALYZE_H_
