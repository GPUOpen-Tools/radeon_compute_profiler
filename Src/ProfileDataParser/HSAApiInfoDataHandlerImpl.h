//==============================================================================
// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief HSAApiInfoDataHandlerImpl Implementation Header
//==============================================================================

#ifndef _HSA_API_INFO_DATA_HANDLER_IMPL_H_
#define _HSA_API_INFO_DATA_HANDLER_IMPL_H_

#include "../HSAFdnTrace/HSAAPIInfo.h"
#include "IHSAApiInfoDataHandler.h"

/// HSA Api Info Data Handler Class
class HSAAPIInfoDataHandler : public IAPIInfoDataHandler,
    public IHSAAPIInfoDataHandler,
    public IHSAMemoryApiInfoDataHandler,
    public IHSAMemoryTransferApiInfoDataHandler,
    public IHSADispatchApiInfoDataHandler
{
public:

    ///Constructor
    /// \param[in] pHsaApiInfo hsa api info
    HSAAPIInfoDataHandler(HSAAPIInfo* pHsaApiInfo): m_pHsaApiInfo(pHsaApiInfo)
    {
    }

    /// Checks the api is HSA memory Api or not
    /// \param[in,opt] ppHsaMemoryApiInfoDataHandler pointer to the hsaMemoryApiInfo if possible otherwise nullptr
    /// \return flag indicating api is HSA memory api or not
    bool IsHSAMemoryApi(IHSAMemoryApiInfoDataHandler** ppHsaMemoryApiInfoDataHandler = nullptr) override;

    /// Checks the api is HSA memory Api or not
    /// \param[in,opt] ppHsaMemoryTransferApiInfoDataHandler pointer to the hsaMemoryTransfer ApiInfo if possible otherwise nullptr
    /// \return flag indicating api is HSA memory transfer api or not
    bool IsHSAMemoryTransferApi(IHSAMemoryTransferApiInfoDataHandler** ppHsaMemoryTransferApiInfoDataHandler = nullptr) override;

    /// Checks the api is HSA memory Api or not
    /// \param[in,opt] ppHsaDispatchApiInfoDataHandler pointer to the hsaDispatchApiInfo if possible otherwise nullptr
    /// \return flag indicating api is HSA dispatch api or not
    bool IsHSADispatchApi(IHSADispatchApiInfoDataHandler** ppHsaDispatchApiInfoDataHandler = nullptr) override;

    /// Returns the casted Common Api Information data handler access pointer
    /// \return api info data handler interface pointer
    IAPIInfoDataHandler* GetApiInfoDataHandler() override;

    /// Gets the api start time
    /// \return start time of the api
    ULONGLONG GetApiStartTime() const override;

    /// Gets the end time of the api
    /// \return end time of the api
    ULONGLONG GetApiEndTime() const override;

    /// Gets the sequence id of the api
    /// \return sequence id of the api
    unsigned int GetApiSequenceId() const override;

    /// Gets the displayable sequence id of the api
    /// \return sequence id for the display
    unsigned int GetApiDisplaySequenceId() const override;

    /// Checks api available for displaybale sequence or not
    /// \return flag indicating api is displayable sequence or not
    bool IsApiSequenceIdDisplayble() const override;

    /// Get the thread id of the api
    /// \return thread id of the api
    osThreadId GetApiThreadId() const override;

    /// Get the return type of the api as char array
    /// \return return type of api
    const char* GetApiRetString() const override;

    /// Get the api name as char array
    /// \return name of the api as char array
    const char* GetApiNameString() const override;

    /// Get the argument list of the api as char array
    /// \return argument list of the api as char array
    const char* GetApiArgListString() const override;

    /// Get the api type of the hsa api
    /// \return hsa api type
    HSA_API_Type GetHSAApiTypeId() const override;

    /// Checks the api is valid or not
    /// \return flag indicating api is valid or not
    bool IsApi() const override;

    /// Get the size of the memory operation
    /// return size of the memory
    size_t GetHSAMemoryApiSize() const override;

    /// Get the start time of the hsa memory transfer operation
    /// \return memory transfer start time
    uint64_t GetHSAMemoryTransferStartTime() const override;

    /// Get the end time of the hsa memory transfer operation
    /// \return memory transfer end time
    uint64_t GetHSAMemoryTransferEndTime() const override;

    /// Get the source agent for the hsa memory transfer
    /// \return source agent
    const char* GetHSASrcAgentString() const override;

    /// Get the destination agent for the hsa memory transfer
    /// \return destination agent
    const char* GetHSADestinationAgentString() const override;

    /// Get the HSA Dispatch Kernel name
    /// \return kernel Name
    const char* GetHSAKernelName() const override;

    /// Get the HSA Dispatch Kernel Handle
    /// \return kernel handle as char array
    unsigned long long GetHSAKernelHandle() const override;

    /// Get the HSA Device Name
    /// \return hsa device name
    const char* GetHSADeviceName() const override;

    /// Get the Device Handle as char array
    /// \return device handle as char array
    const char* GetHSADeviceHandleString() const override;

    /// Get the HSA Queue Index
    /// \return queue index
    unsigned int GetHSAQueueIndex() const override;

    /// Get the HSA Queue index handle as char array
    /// \return hsa queue handle as char array
    const char* GetHSAQueueHandleString() const override;

    /// Get the HSA Work Group size
    /// \return hsa work group size
    const char* GetHSAWorkGroupSizeString() const override;

    /// Get the HSA global work group size
    /// \return global work group size
    const char* GetHSAGlobalWorkGroupSize() const override;

    /// Get the HSA local groups
    /// \param[out] ppHsaLocalGroups hsa local groups
    /// \param[out] groupCount number of groups
    void GetHSALocalGroups(size_t** ppHsaLocalGroups, size_t& groupCount) const  override;

    /// Get the HSA global groups
    /// \param[out] ppHsaGlobalGroups hsa global groups
    /// \param[out] groupCount number of groups
    void GetHSAGlobalGroups(size_t** ppHsaGlobalGroups, size_t& groupCount) const  override;

    /// Get the HSA group dimension
    /// \return dimension of the group
    size_t GetHSAGroupDimension() const override;

    /// Virtual Destructor
    ~HSAAPIInfoDataHandler();

private:

    HSAAPIInfo*                                 m_pHsaApiInfo;          ///< pointer to hsa api info
    mutable std::vector<size_t*>                m_pGroupList;           ///< list of dynamically allocated pointer for hsa groups
};

#endif // _HSA_API_INFO_DATA_HANDLER_IMPL_H_
