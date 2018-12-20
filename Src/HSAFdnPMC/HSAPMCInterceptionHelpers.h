//==============================================================================
// Copyright (c) 2015-2018 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  This file contains functions called by various intercepted APIs
//==============================================================================

#ifndef _HSA_PMC_INTERCEPTION_HELPERS_
#define _HSA_PMC_INTERCEPTION_HELPERS_

#include "hsa.h"

/// interception helper function for hsa_queue_create
/// see HSA runtime spec for parameter descriptions
void HSA_PMC_hsa_queue_create_PostCallHelper(hsa_status_t retVal, hsa_agent_t agent, uint32_t size, hsa_queue_type32_t type, void(*callback)(hsa_status_t status, hsa_queue_t* source,
                                             void* data), void* data, uint32_t private_segment_size, uint32_t group_segment_size, hsa_queue_t** queue);

/// interception helper function for hsa_queue_destroy
/// see HSA runtime spec for parameter descriptions
void HSA_PMC_hsa_queue_destroy_PreCallHelper(hsa_queue_t* queue);

/// interception helper function for hsa_executable_get_symbol
/// see HSA runtime spec for parameter descriptions
void HSA_PMC_hsa_executable_get_symbol_PostCallHelper(hsa_status_t retVal, hsa_executable_t executable, const char* module_name, const char* symbol_name, hsa_agent_t agent, int32_t call_convention, hsa_executable_symbol_t* symbol);

/// interception helper function for hsa_executable_get_symbol_by_name
/// see HSA runtime spec for parameter descriptions
void HSA_PMC_hsa_executable_get_symbol_by_name_PostCallHelper(hsa_status_t retVal, hsa_executable_t executable, const char* symbol_name, const hsa_agent_t* agent, hsa_executable_symbol_t* symbol);

/// interception helper function for hsa_executable_symbol_get_info
/// see HSA runtime spec for parameter descriptions
void HSA_PMC_hsa_executable_symbol_get_info_PostCallHelper(hsa_status_t retVal, hsa_executable_symbol_t executable_symbol, hsa_executable_symbol_info_t attribute, void* value);

/// interception helper function for hsa_executable_load_code_object
/// see HSA runtime spec for parameter descriptions
void HSA_PMC_hsa_executable_load_code_object_PostCallHelper(hsa_status_t retVal, hsa_executable_t executable, hsa_agent_t agent, hsa_code_object_t code_object, const char* options);

/// interception helper function for hsa_executable_load_agent_code_object
/// see HSA runtime spec for parameter descriptions
void HSA_PMC_hsa_executable_load_agent_code_object_PostCallHelper(hsa_status_t retVal, hsa_executable_t executable, hsa_agent_t agent, hsa_code_object_reader_t code_object_reader, const char* options, hsa_loaded_code_object_t* loaded_code_object);

#endif // _HSA_PMC_INTERCEPTION_HELPERS_
