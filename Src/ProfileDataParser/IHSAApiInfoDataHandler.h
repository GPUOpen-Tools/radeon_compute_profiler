//==============================================================================
// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief HSA Api Info Interface header
//==============================================================================

#ifndef _I_HSA_API_INFO_DATA_HANDLER_H_
#define _I_HSA_API_INFO_DATA_HANDLER_H_

// HSA common
#include <HSAFunctionDefs.h>

#include "IAPIInfoDataHandler.h"

class IHSAMemoryApiInfoDataHandler;
class IHSAMemoryTransferApiInfoDataHandler;
class IHSADispatchApiInfoDataHandler;

/// HSA Api Info Data Handler Interface
class IHSAAPIInfoDataHandler
{
public:
    /// Checks the api is HSA memory Api or not
    /// \param[in,opt] ppHsaMemoryApiInfoDataHandler pointer to the hsaMemoryApiInfo if possible otherwise nullptr
    /// \return flag indicating api is HSA memory api or not
    virtual bool IsHSAMemoryApi(IHSAMemoryApiInfoDataHandler** ppHsaMemoryApiInfoDataHandler = nullptr) = 0;

    /// Checks the api is HSA memory Api or not
    /// \param[in,opt] ppHsaMemoryTransferApiInfoDataHandler pointer to the hsaMemoryTransfer ApiInfo if possible otherwise nullptr
    /// \return flag indicating api is HSA memory transfer api or not
    virtual bool IsHSAMemoryTransferApi(IHSAMemoryTransferApiInfoDataHandler** ppHsaMemoryTransferApiInfoDataHandler = nullptr) = 0;

    /// Checks the api is HSA dispatch Api or not
    /// \param[in,opt] ppHsaDispatchApiInfoDataHandler pointer to the hsaDispatchApiInfo if possible otherwise nullptr
    /// \return flag indicating api is HSA dispatch api or not
    virtual bool IsHSADispatchApi(IHSADispatchApiInfoDataHandler** ppHsaDispatchApiInfoDataHandler = nullptr) = 0;

    /// Returns the casted Common Api Information data handler access pointer
    /// \return api info data handler interface pointer
    virtual IAPIInfoDataHandler* GetApiInfoDataHandler() = 0;

    /// Get the api type of the hsa api
    /// \return hsa api type
    virtual HSA_API_Type GetHSAApiTypeId() const = 0;

    /// Checks the api is valid or not
    /// \return flag indicating api is valid or not
    virtual bool IsApi() const = 0;

    /// Virtual Destructor
    virtual ~IHSAAPIInfoDataHandler() = default;
};


/// HSA Memory Api Info Data Handler Interface
class IHSAMemoryApiInfoDataHandler
{
public:
    /// Get the size of the memory operation
    /// \return size of the memory
    virtual size_t GetHSAMemoryApiSize() const = 0;

    /// Virtual Destructor
    virtual ~IHSAMemoryApiInfoDataHandler() = default;
};


/// HSA Memory Transfer Api Info Data Handler Interface
class IHSAMemoryTransferApiInfoDataHandler
{
public:
    /// Get the start time of the hsa memory transfer operation
    /// \return memory transfer start time
    virtual uint64_t GetHSAMemoryTransferStartTime() const = 0;

    /// Get the end time of the hsa memory transfer operation
    /// \return memory transfer end time
    virtual uint64_t GetHSAMemoryTransferEndTime() const = 0;

    /// Get the source agent for the hsa memory transfer
    /// \return source agent
    virtual const char* GetHSASrcAgentString() const = 0;

    /// Get the destination agent for the hsa memory transfer
    /// \return destination agent
    virtual const char* GetHSADestinationAgentString() const = 0;

    /// Virtual Destructor
    virtual ~IHSAMemoryTransferApiInfoDataHandler() = default;
};


/// HSA Dispatch Api Info Data Handler Interface
class IHSADispatchApiInfoDataHandler
{
public:
    /// Get the HSA Dispatch Kernel name
    /// \return kernel Name
    virtual const char* GetHSAKernelName() const = 0;

    /// Get the HSA Dispatch Kernel Handle
    /// \return kernel handle
    virtual ULONGLONG GetHSAKernelHandle() const = 0;

    /// Get the HSA Device Name
    /// \return hsa device name as char array
    virtual const char* GetHSADeviceName() const = 0;

    /// Get the Device Handle as char array
    /// \return device handle as char array
    virtual const char* GetHSADeviceHandleString() const = 0;

    /// Get the HSA Queue Index
    /// \return queue index
    virtual unsigned int GetHSAQueueIndex() const = 0;

    /// Get the HSA Queue index handle as char array
    /// \return hsa queue handle as char array
    virtual const char* GetHSAQueueHandleString() const = 0;

    /// Get the HSA Work Group size
    /// \return hsa work group size
    virtual const char* GetHSAWorkGroupSizeString() const = 0;

    /// Get the HSA global work group size
    /// \return global work group size
    virtual const char* GetHSAGlobalWorkGroupSize() const = 0;

    /// Get the HSA local groups
    /// \param[out] ppHsaLocalGroups hsa local groups
    /// \param[out] groupCount number of groups
    virtual void GetHSALocalGroups(size_t** ppHsaLocalGroups, size_t& groupCount) const = 0;

    /// Get the HSA global groups
    /// \param[out] ppHsaGlobalGroups hsa global groups
    /// \param[out] groupCount number of groups
    virtual void GetHSAGlobalGroups(size_t** ppHsaGlobalGroups, size_t& groupCount) const = 0;

    /// Get the HSA group dimension
    /// \return dimension of the group
    virtual size_t GetHSAGroupDimension() const = 0;

    /// Virtual Destructor
    virtual ~IHSADispatchApiInfoDataHandler() = default;
};


#endif // _I_HSA_API_INFO_DATA_HANDLER_H_
