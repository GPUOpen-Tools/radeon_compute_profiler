//==============================================================================
// Copyright (c) 2016 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  This file provides kernel name demangling helper function
//==============================================================================

#ifndef _HSA_KERNEL_DEMANGLER_H_
#define _HSA_KERNEL_DEMANGLER_H_

#include <string>

/// Demangles the kernel name if mangled
/// \param kernelname kernel name
/// \return demangled kernel name using c++filt
std::string DemangleKernelName(std::string& kernelName);

#endif