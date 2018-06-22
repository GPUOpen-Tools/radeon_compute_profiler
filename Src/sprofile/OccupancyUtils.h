//==============================================================================
// Copyright (c) 2015 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief File utils functions specifically for occupancy files
//==============================================================================

#ifndef _OCCUPANCY_UTILS_H_
#define _OCCUPANCY_UTILS_H_

#include "../ProfileDataParser/OccupancyParams.h"

namespace OccupancyUtils
{
/// Loads the occupancy params from an occupancy config file
/// \param strOccupancyFile the occupancy params file with name-value pairs
/// \params[out] the occupancy parameters loaded from the file
/// \params[out] an error message
/// \return true on success, false otehrwise
bool GetOccupancyParamsFromFile(const std::string& strOccupancyFile, OccupancyParams& params, std::string& strError);

/// Loads the occupancy params from an occupancy config file
/// \param strOccupancyFile an .occupancy file output by the profiler
/// \param occupancyIndex the index of the kernel in the file
/// \params[out] the occupancy parameters loaded from the file
/// \params[out] an error message
/// \return true on success, false otehrwise
bool GetOccupancyParamsFromFile(const std::string& strOccupancyFile, unsigned int occupancyIndex, OccupancyParams& params, std::string& strError);
}
#endif
