//==============================================================================
// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Kernel Occupancy Info Data Handler Interface header
//==============================================================================

#ifndef _I_OCCUPANCY_INFO_DATA_HANDLER_H_
#define _I_OCCUPANCY_INFO_DATA_HANDLER_H_

// std
#include <string>

// common
#include <DeviceInfo.h>

/// Kernel Occupancy Info Data Handler interface
class IOccupancyInfoDataHandler
{
public:

    /// Get the thread Id
    /// \return thread Id
    virtual unsigned int GetThreadId() const = 0;

    /// Get the kernel name
    /// \return kernel name char array
    virtual const char* GetKernelName() const = 0;

    /// Get the device name
    /// \return device name char array
    virtual const char* GetDeviceName() const = 0;

    /// Get the Graphics ip version of the device
    /// \return gfxIp of the device
    virtual unsigned int GetDeviceGfxIp() const = 0;

    /// Get the count of the compute unit on device
    /// \return number of the CU on device
    virtual unsigned int GetComputeUnitCount() const = 0;

    /// Get the maximum wave count per CU
    /// \return maximum count of the waves in a CU
    virtual unsigned int GetMaxWavePerCU() const = 0;

    /// Get max number of work group per compute unit
    /// \return max number of work group per compute unit
    virtual unsigned int GetMaxWorkGroupPerCU() const = 0;

    /// Get Max number of vector GPR on compute unit
    /// \return Max number of vector GPR on compute unit
    virtual unsigned int GetMaxVGPRs() const = 0;

    /// Get Max number of scalar GPR on compute unit
    /// \return Max number of scalar GPR on compute unit
    virtual unsigned int GetMaxSGPRs() const = 0;

    /// Get Max amount of LDS on compute unit
    /// \return Max amount of LDS on compute unit
    virtual unsigned int GetMaxLDS() const = 0;

    /// Get Number of vector GPR used by kernel
    /// \return Number of vector GPR used by kernel
    virtual unsigned int GetUsedVGPRs() const = 0;

    /// Get Number of scalar GPR used by kernel
    /// \return Number of scalar GPR used by kernel
    virtual unsigned int GetUsedSGPRs() const = 0;

    /// Get Amount of LDS used by kernel (per work-group)
    /// \return Amount of LDS used by kernel (per work-group)
    virtual unsigned int GetUsedLDS() const = 0;

    /// Get Number of work-items per work-group - wave front size
    /// \return Number of work-items per work-group - wave front size
    virtual unsigned int GetWavefrontSize() const = 0;

    /// Get Number of work-items in work-group
    /// \return Number of work-items in work-group
    virtual unsigned int GetWorkGroupSize() const = 0;

    /// Get Number of wavefronts in work-group
    /// \return Number of wavefronts in work-group
    virtual unsigned int GetWavesPerWorkGroup() const = 0;

    /// Get Max. number of work-items in a work-group
    /// \return Max. number of work-items in a work-group
    virtual unsigned int GetMaxWorkGroupSize() const = 0;

    /// Get Max. number of waves per work-group
    /// \return Max. number of waves per work-group
    virtual unsigned int GetMaxWavesPerWorkGroup() const = 0;

    /// Get Global number work items
    /// \return Global number work items
    virtual unsigned int GetGlobalWorkSize() const = 0;

    /// Get Global max. number of work-items
    /// \return Global max. number of work-items
    virtual unsigned int GetMaxGloablWorkSize() const = 0;

    /// Get Number of wavefronts when the VGPR is the only limit on resources
    /// \return Number of wavefronts
    virtual unsigned int GetWavesLimitByVGPR() const = 0;

    /// Get Number of wavefronts when the SGPR is the only limit on resources
    /// \return Number of wavefronts
    virtual unsigned int GetWavesLimitBySGPR() const = 0;

    /// Get Number of wavefronts when the shared memory is the only limit on resources
    /// \return Number of wavefronts
    virtual unsigned int GetWavesLimitByLDS() const = 0;

    /// Get Number of wavefronts when the work-group size is the only constraint on resources
    /// \return Number of wavefronts
    virtual unsigned int GetWavesLimitByWorkGroup() const = 0;

    /// Get compute unit occupancy
    /// \return compute unit
    virtual float GetOccupancy() const = 0;

    /// Get ASIC generation
    /// \return ASIC generation
    virtual GDT_HW_GENERATION GetGeneration() const = 0;

    /// Get the number of SIMDS per CU
    /// \return number of SIMDS per CU
    virtual unsigned int GetSimdsPerCU() const = 0;

    /// Generate the Occupancy Chart HTML Files
    /// \param[in] pOutputFile occupancy chart output file
    /// \param[out] ppErrorMessage error message if operation was not successful
    /// \return flag indicating the operation is successful or not
    virtual bool GenerateOccupancyChart(const char* pOutputFile, char** ppErrorMessage) const = 0;

    /// Virtual Destructor
    virtual ~IOccupancyInfoDataHandler() = default;
};

#endif // _I_OCCUPANCY_INFO_DATA_HANDLER_H_
