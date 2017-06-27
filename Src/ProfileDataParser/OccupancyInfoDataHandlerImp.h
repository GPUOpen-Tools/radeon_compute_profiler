//==============================================================================
// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Occupancy Info Data Handler Implmentation header
//==============================================================================

#ifndef _OCCUPANCY_INFO_DATA_HANDLER_IMP_H_
#define _OCCUPANCY_INFO_DATA_HANDLER_IMP_H_

#include "IOccupancyInfoDataHandler.h"
#include "../sprofile/OccupancyUtils.h"

/// Kernel occupancy info data handler class
class OccupancyInfoDataHandler : public IOccupancyInfoDataHandler
{
public:

    ///Constructor
    OccupancyInfoDataHandler() : m_pErrorMessageResource(nullptr)
    {
    }

    /// Get the thread Id
    /// \return thread Id
    unsigned int GetThreadId() const override;

    /// Get the kernel name
    /// \return kernel name char array
    const char* GetKernelName() const override;

    /// Get the device name
    /// \return device name char array
    const char* GetDeviceName() const override;

    /// Get the Graphics ip version of the device
    /// \return gfxIp of the device
    unsigned int GetDeviceGfxIp() const override;

    /// Get the count of the compute unit on device
    /// \return number of the CU on device
    unsigned int GetComputeUnitCount() const override;

    /// Get the maximum wave count per CU
    /// \return maximum count of the waves in a CU
    unsigned int GetMaxWavePerCU() const override;

    /// Get max number of work group per compute unit
    /// \return max number of work group per compute unit
    unsigned int GetMaxWorkGroupPerCU() const override;

    /// Get Max number of vector GPR on compute unit
    /// \return Max number of vector GPR on compute unit
    unsigned int GetMaxVGPRs() const override;

    /// Get Max number of scalar GPR on compute unit
    /// \return Max number of scalar GPR on compute unit
    unsigned int GetMaxSGPRs() const override;

    /// Get Max amount of LDS on compute unit
    /// \return Max amount of LDS on compute unit
    unsigned int GetMaxLDS() const override;

    /// Get Number of vector GPR used by kernel
    /// \return Number of vector GPR used by kernel
    unsigned int GetUsedVGPRs() const override;

    /// Get Number of scalar GPR used by kernel
    /// \return Number of scalar GPR used by kernel
    unsigned int GetUsedSGPRs() const override;

    /// Get Amount of LDS used by kernel (per work-group)
    /// \return Amount of LDS used by kernel (per work-group)
    unsigned int GetUsedLDS() const override;

    /// Get Number of work-items per work-group - wave front size
    /// \return Number of work-items per work-group - wave front size
    unsigned int GetWavefrontSize() const override;

    /// Get Number of work-items in work-group
    /// \return Number of work-items in work-group
    unsigned int GetWorkGroupSize() const override;

    /// Get Number of wavefronts in work-group
    /// \return Number of wavefronts in work-group
    unsigned int GetWavesPerWorkGroup() const override;

    /// Get Max. number of work-items in a work-group
    /// \return Max. number of work-items in a work-group
    unsigned int GetMaxWorkGroupSize() const override;

    /// Get Max. number of waves per work-group
    /// \return Max. number of waves per work-group
    unsigned int GetMaxWavesPerWorkGroup() const override;

    /// Get Global number work items
    /// \return Global number work items
    unsigned int GetGlobalWorkSize() const override;

    /// Get Global max. number of work-items
    /// \return Global max. number of work-items
    unsigned int GetMaxGloablWorkSize() const override;

    /// Get Number of wavefronts when the VGPR is the only limit on resources
    /// \return Number of wavefronts
    unsigned int GetWavesLimitByVGPR() const override;

    /// Get Number of wavefronts when the SGPR is the only limit on resources
    /// \return Number of wavefronts
    unsigned int GetWavesLimitBySGPR() const override;

    /// Get Number of wavefronts when the shared memory is the only limit on resources
    /// \return Number of wavefronts
    unsigned int GetWavesLimitByLDS() const override;

    /// Get Number of wavefronts when the work-group size is the only constraint on resources
    /// \return Number of wavefronts
    unsigned int GetWavesLimitByWorkGroup() const override;

    /// Get compute unit occupancy
    /// \return compute unit
    float GetOccupancy() const override;

    /// Get ASIC generation
    /// \return generation
    GDT_HW_GENERATION GetGeneration() const override;

    /// Set the number of SIMDS per CU
    /// \return number of SIMDS per CU
    unsigned int GetSimdsPerCU() const override;

    /// Generate the Occupancy Chart HTML Files
    /// \param[in] pOutputFile occupancy chart output file
    /// \param[out] ppErrorMessage error message if operation was not successful
    /// \return flag indicating the operation is successful or not
    bool GenerateOccupancyChart(const char* pOutputFile, char** ppErrorMessage) const override;

    /// Virtual Destructor
    ~OccupancyInfoDataHandler();

    OccupancyUtils::OccupancyParams m_occupancyInfo;        ///< occupancy info struct member

private:
    mutable char* m_pErrorMessageResource;                  ///< error message resource for handling message on generating occupancy chart
};

#endif // _OCCUPANCY_INFO_DATA_HANDLER_IMP_H_
