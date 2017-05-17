//==============================================================================
// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief CL Api Info Interface header
//==============================================================================

#ifndef _I_CL_API_INFO_DATA_HANDLER_H_
#define _I_CL_API_INFO_DATA_HANDLER_H_

// CL common
#include <CLFunctionEnumDefs.h>
#include <Defs.h>

#include "IAPIInfoDataHandler.h"

class ICLMemApiInfoDataHandler;
class ICLOtherEnqueueApiInfoDataHandler;
class ICLEnqueueApiInfoDataHandler;
class ICLDataEnqueueApiInfoDataHandler;
class ICLKernelApiInfoDataHandler;


/// CL Api Info data handler Interface
class ICLAPIInfoDataHandler
{
public:
    /// Checks clapiinfo is CLMemApiInfo or not
    /// \param[in,opt] ppClMemApiInfoDataHandler interface pointer to the clMemApiInfo if exists otherwise nullptr
    /// \return flag indicating clMempApiInfo or not
    virtual bool IsCLMemoryApiInfo(ICLMemApiInfoDataHandler** ppClMemApiInfoDataHandler = nullptr) = 0;

    /// Checks clapiinfo is clEnqueueOtheroperations or not
    /// \param[in,opt] ppClEnqueueOtherOperations interface pointer to the clEnqueueOtheroperations if exists otherwise nullptr
    /// \return flag indicating clMempApiInfo or not
    virtual bool IsCLEnqueueOtherOperations(ICLOtherEnqueueApiInfoDataHandler** ppClEnqueueOtherOperations = nullptr) = 0;

    /// Checks clapiinfo is clEnqueue api or not
    /// \param[in,opt] ppClEnqueueApiInfoDataHandler interface pointer to the clEnqueue if exists otherwise nullptr
    /// \return flag indicating clMempApiInfo or not
    virtual bool IsCLEnqueueAPI(ICLEnqueueApiInfoDataHandler** ppClEnqueueApiInfoDataHandler = nullptr) = 0;

    /// Checks clapiinfo is clDataEnqueue api or not
    /// \param[in,opt] ppClDataEnqueueApiInfoDataHandler interface pointer to the clDataEnqueue if exists otherwise nullptr
    /// \return flag indicating clMempApiInfo or not
    virtual bool IsCLDataEnqueueApi(ICLDataEnqueueApiInfoDataHandler** ppClDataEnqueueApiInfoDataHandler = nullptr) = 0;

    /// Checks clapiinfo is clKernelApiInfo or not
    /// \param[in,opt] ppClKernelApiInfoDataHandler interface pointer to the clKernelApiInfo if exists otherwise nullptr
    /// \return flag indicating clMempApiInfo or not
    virtual bool IsCLKernelApiInfo(ICLKernelApiInfoDataHandler** ppClKernelApiInfoDataHandler = nullptr) = 0;

    /// Returns the casted Common Api Information data handler access pointer
    /// \return api info data handler interface pointer
    virtual IAPIInfoDataHandler* GetApiInfoDataHandler() = 0;

    /// Gets the cl api type
    /// \return api Type
    virtual CLAPIType GetCLApiType() const = 0;

    /// Get the OpenCl Api Id
    /// \return api id
    virtual unsigned int GetCLApiId() const = 0;

    /// Get the comment string
    /// \return comment String
    virtual std::string GetCLApiComment() const = 0;

    /// Virtual Destructor
    virtual ~ICLAPIInfoDataHandler() = default;
};


/// CL Enqueue Api Info Data Handler Interface
class ICLEnqueueApiInfoDataHandler
{
public:
    /// Checks the Enqueue information is missing or not
    /// \return flag indicating Enqueue information is present or not
    virtual bool IsCLEnqueueInformationMissing() const = 0;

    /// Get the CLEnqueue command type string
    /// \return command type string
    virtual std::string GetCLCommandTypeString() const = 0;

    /// Get the CL Device name string
    /// \return cl device name
    virtual std::string GetCLDeviceNameString() const = 0;

    /// Get the CL context handle string
    /// \return cl context handle string
    virtual std::string GetCLContextHandleString() const = 0;

    /// Get the CL Command Queue Handle string
    /// \return cl command queue handle string
    virtual std::string GetCLCommandQueueHandleString() const = 0;

    /// Get the cl event handle string
    /// \return cl event hadnle string
    virtual std::string GetCLEventHandleString() const = 0;

    /// Get the CL queue timestamp
    /// \return cl queue timestamp
    virtual ULONGLONG GetCLQueueTimestamp() const = 0;

    /// Get the CL submit timestamp
    /// \return cl submit timestamp
    virtual ULONGLONG GetCLSubmitTimestamp() const = 0;

    /// Get the CL running timestamp
    /// \return cl running timestamp
    virtual ULONGLONG GetCLRunningTimestamp() const = 0;

    /// Get the CL complete timestamp
    /// \return cl complete timestamp
    virtual ULONGLONG GetCLCompleteTimestamp() const = 0;

    /// Get the CL command type enum
    /// \return cl command type enum
    virtual unsigned int GetCLCommandTypeEnum() const = 0;

    /// Get the CL queue id
    /// \return cl queue id
    virtual unsigned int GetCLQueueId() const = 0;

    /// Get the CL context id
    /// \return cl context id
    virtual unsigned int GetCLContextId() const = 0;

    /// Virtual Destructor
    virtual ~ICLEnqueueApiInfoDataHandler() = default;
};


/// CL Memory Api Info Data Handler Interface
class ICLMemApiInfoDataHandler
{
public:
    /// Get the CL memory Transfer size
    /// \return cl memory size
    virtual unsigned int GetCLMemoryTransferSize() const = 0;

    /// Virtual Destructor
    virtual ~ICLMemApiInfoDataHandler() = default;

};


/// CL Other Enqueue Api Info Data Handler Interface
class ICLOtherEnqueueApiInfoDataHandler
{
public:
    /// Virtual Destructor
    virtual ~ICLOtherEnqueueApiInfoDataHandler() = default;
};


/// CL Data Enqueue Api Info Data Handler Interface
class ICLDataEnqueueApiInfoDataHandler
{
public:
    /// Get the CL Data transfer size
    /// \return data transfer size
    virtual unsigned int GetCLDataTransferSize() const = 0;

    /// Virtual Destructor
    virtual ~ICLDataEnqueueApiInfoDataHandler() = default;
};


/// CL Kernel Api Info Data Handler Interface
class ICLKernelApiInfoDataHandler
{
public:
    /// Get the CL kernel handle String
    /// \return kernel handle as string
    virtual std::string GetCLKernelHandleString() const = 0;

    /// Get the CL Kernel name
    /// \return kernel name
    virtual std::string GetCLKernelNameString() const = 0;

    /// Get the CL kernel work group size
    /// \return kernel work group size
    virtual std::string GetCLKernelWorkGroupSize() const = 0;

    /// Get the CL global work group size
    /// \return kernel global work group size
    virtual std::string GetCLKernelGlobalWorkGroupSize() const = 0;

    /// Virtual Destructor
    virtual ~ICLKernelApiInfoDataHandler() = default;
};

#endif // _I_CL_API_INFO_DATA_HANDLER_H_
