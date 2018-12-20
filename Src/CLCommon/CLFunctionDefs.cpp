//==============================================================================
// Copyright (c) 2015 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief The helper file to initialize the OpenCL function pointers.
//==============================================================================

#include <algorithm>
#include <cstring>
#include <fstream>

#include <AMDTBaseTools/Include/AMDTDefinitions.h>

#include <AMDTOSWrappers/Include/osModule.h>
#include <AMDTOSWrappers/Include/osFilePath.h>

#include "../Common/OSUtils.h"
#include "../Common/Logger.h"
#include "../Common/FileUtils.h"

#include "CLFunctionDefs.h"
#include "OpenCLModule.h"

cl_icd_dispatch_table g_nextDispatchTable;
cl_icd_dispatch_table g_realDispatchTable;

CLExtensionFunctionTable g_realExtensionFunctionTable;

void InitNextCLFunctions(cl_icd_dispatch_table* pNextTable, cl_icd_dispatch_table* pRealTable)
{
    SpAssertRet(nullptr != pNextTable);

    InitRealCLFunctions(pRealTable);

    memcpy(&g_nextDispatchTable, pNextTable, sizeof(g_nextDispatchTable));
}

void InitRealCLFunctions(cl_icd_dispatch_table* pRealTable)
{
    static OpenCLModule g_OpenCLModule;

    if (g_OpenCLModule.OpenCLLoaded() == OpenCLModule::OpenCL_None)
    {
        return;
    }

    // if thisModuleName is empty, then we are not initializing the real dispatch table with
    // an agent's original dispatch table. Thus, we load the entry points from opencl.dll or libopencl.so
    if (nullptr == pRealTable)
    {
        g_realDispatchTable.GetPlatformInfo = g_OpenCLModule.GetPlatformInfo;
        g_realDispatchTable.GetDeviceIDs = g_OpenCLModule.GetDeviceIDs;
        g_realDispatchTable.GetDeviceInfo = g_OpenCLModule.GetDeviceInfo;
        g_realDispatchTable.CreateContext = g_OpenCLModule.CreateContext;
        g_realDispatchTable.CreateContextFromType = g_OpenCLModule.CreateContextFromType;
        g_realDispatchTable.RetainContext = g_OpenCLModule.RetainContext;
        g_realDispatchTable.ReleaseContext = g_OpenCLModule.ReleaseContext;
        g_realDispatchTable.GetContextInfo = g_OpenCLModule.GetContextInfo;
        g_realDispatchTable.CreateCommandQueue = g_OpenCLModule.CreateCommandQueue;
        g_realDispatchTable.RetainCommandQueue = g_OpenCLModule.RetainCommandQueue;
        g_realDispatchTable.ReleaseCommandQueue = g_OpenCLModule.ReleaseCommandQueue;
        g_realDispatchTable.GetCommandQueueInfo = g_OpenCLModule.GetCommandQueueInfo;
        g_realDispatchTable.SetCommandQueueProperty = g_OpenCLModule.SetCommandQueueProperty;
        g_realDispatchTable.CreateProgramWithSource = g_OpenCLModule.CreateProgramWithSource;
        g_realDispatchTable.CreateProgramWithBinary = g_OpenCLModule.CreateProgramWithBinary;
        g_realDispatchTable.RetainProgram = g_OpenCLModule.RetainProgram;
        g_realDispatchTable.ReleaseProgram = g_OpenCLModule.ReleaseProgram;
        g_realDispatchTable.BuildProgram = g_OpenCLModule.BuildProgram;
        g_realDispatchTable.UnloadCompiler = g_OpenCLModule.UnloadCompiler;
        g_realDispatchTable.GetProgramInfo = g_OpenCLModule.GetProgramInfo;
        g_realDispatchTable.GetProgramBuildInfo = g_OpenCLModule.GetProgramBuildInfo;
        g_realDispatchTable.CreateKernel = g_OpenCLModule.CreateKernel;
        g_realDispatchTable.CreateKernelsInProgram = g_OpenCLModule.CreateKernelsInProgram;
        g_realDispatchTable.RetainKernel = g_OpenCLModule.RetainKernel;
        g_realDispatchTable.ReleaseKernel = g_OpenCLModule.ReleaseKernel;
        g_realDispatchTable.SetKernelArg = g_OpenCLModule.SetKernelArg;
        g_realDispatchTable.GetKernelInfo = g_OpenCLModule.GetKernelInfo;
        g_realDispatchTable.GetKernelWorkGroupInfo = g_OpenCLModule.GetKernelWorkGroupInfo;
        g_realDispatchTable.WaitForEvents = g_OpenCLModule.WaitForEvents;
        g_realDispatchTable.GetEventInfo = g_OpenCLModule.GetEventInfo;
        g_realDispatchTable.RetainEvent = g_OpenCLModule.RetainEvent;
        g_realDispatchTable.ReleaseEvent = g_OpenCLModule.ReleaseEvent;
        g_realDispatchTable.GetEventProfilingInfo = g_OpenCLModule.GetEventProfilingInfo;
        g_realDispatchTable.Flush = g_OpenCLModule.Flush;
        g_realDispatchTable.Finish = g_OpenCLModule.Finish;
        g_realDispatchTable.EnqueueNDRangeKernel = g_OpenCLModule.EnqueueNDRangeKernel;
        g_realDispatchTable.EnqueueTask = g_OpenCLModule.EnqueueTask;
        g_realDispatchTable.EnqueueNativeKernel = g_OpenCLModule.EnqueueNativeKernel;
        g_realDispatchTable.EnqueueMarker = g_OpenCLModule.EnqueueMarker;
        g_realDispatchTable.EnqueueWaitForEvents = g_OpenCLModule.EnqueueWaitForEvents;
        g_realDispatchTable.EnqueueBarrier = g_OpenCLModule.EnqueueBarrier;
        g_realDispatchTable.CreateBuffer = g_OpenCLModule.CreateBuffer;
        g_realDispatchTable.CreateImage2D = g_OpenCLModule.CreateImage2D;
        g_realDispatchTable.CreateImage3D = g_OpenCLModule.CreateImage3D;
        g_realDispatchTable.RetainMemObject = g_OpenCLModule.RetainMemObject;
        g_realDispatchTable.ReleaseMemObject = g_OpenCLModule.ReleaseMemObject;
        g_realDispatchTable.GetSupportedImageFormats = g_OpenCLModule.GetSupportedImageFormats;
        g_realDispatchTable.GetMemObjectInfo = g_OpenCLModule.GetMemObjectInfo;
        g_realDispatchTable.GetImageInfo = g_OpenCLModule.GetImageInfo;
        g_realDispatchTable.CreateSampler = g_OpenCLModule.CreateSampler;
        g_realDispatchTable.RetainSampler = g_OpenCLModule.RetainSampler;
        g_realDispatchTable.ReleaseSampler = g_OpenCLModule.ReleaseSampler;
        g_realDispatchTable.GetSamplerInfo = g_OpenCLModule.GetSamplerInfo;
        g_realDispatchTable.EnqueueReadBuffer = g_OpenCLModule.EnqueueReadBuffer;
        g_realDispatchTable.EnqueueWriteBuffer = g_OpenCLModule.EnqueueWriteBuffer;
        g_realDispatchTable.EnqueueReadImage = g_OpenCLModule.EnqueueReadImage;
        g_realDispatchTable.EnqueueWriteImage = g_OpenCLModule.EnqueueWriteImage;
        g_realDispatchTable.EnqueueMapBuffer = g_OpenCLModule.EnqueueMapBuffer;
        g_realDispatchTable.EnqueueMapImage = g_OpenCLModule.EnqueueMapImage;
        g_realDispatchTable.EnqueueUnmapMemObject = g_OpenCLModule.EnqueueUnmapMemObject;
        g_realDispatchTable.EnqueueCopyBuffer = g_OpenCLModule.EnqueueCopyBuffer;
        g_realDispatchTable.EnqueueCopyImage = g_OpenCLModule.EnqueueCopyImage;
        g_realDispatchTable.EnqueueCopyImageToBuffer = g_OpenCLModule.EnqueueCopyImageToBuffer;
        g_realDispatchTable.EnqueueCopyBufferToImage = g_OpenCLModule.EnqueueCopyBufferToImage;
        // OpenCL 1.1
        g_realDispatchTable.CreateUserEvent = g_OpenCLModule.CreateUserEvent;
        g_realDispatchTable.SetUserEventStatus = g_OpenCLModule.SetUserEventStatus;
        g_realDispatchTable.SetEventCallback = g_OpenCLModule.SetEventCallback;
        g_realDispatchTable.CreateSubBuffer = g_OpenCLModule.CreateSubBuffer;
        g_realDispatchTable.SetMemObjectDestructorCallback = g_OpenCLModule.SetMemObjectDestructorCallback;
        g_realDispatchTable.EnqueueReadBufferRect = g_OpenCLModule.EnqueueReadBufferRect;
        g_realDispatchTable.EnqueueWriteBufferRect = g_OpenCLModule.EnqueueWriteBufferRect;
        g_realDispatchTable.EnqueueCopyBufferRect = g_OpenCLModule.EnqueueCopyBufferRect;

        g_realDispatchTable.CreateFromGLBuffer = g_OpenCLModule.CreateFromGLBuffer;
        g_realDispatchTable.CreateFromGLTexture2D = g_OpenCLModule.CreateFromGLTexture2D;
        g_realDispatchTable.CreateFromGLTexture3D = g_OpenCLModule.CreateFromGLTexture3D;
        g_realDispatchTable.CreateFromGLRenderbuffer = g_OpenCLModule.CreateFromGLRenderbuffer;
        g_realDispatchTable.GetGLObjectInfo = g_OpenCLModule.GetGLObjectInfo;
        g_realDispatchTable.GetGLTextureInfo = g_OpenCLModule.GetGLTextureInfo;
        g_realDispatchTable.EnqueueAcquireGLObjects = g_OpenCLModule.EnqueueAcquireGLObjects;
        g_realDispatchTable.EnqueueReleaseGLObjects = g_OpenCLModule.EnqueueReleaseGLObjects;
        g_realDispatchTable.GetGLContextInfoKHR = g_OpenCLModule.GetGLContextInfoKHR;
        // clCreateEventFromGLsyncKHR is not an addition to OpenCL 1.2, but the
        // dispatch table in pre 1.2 versions did not contain an entry for this API
        g_realDispatchTable.CreateEventFromGLsyncKHR = g_OpenCLModule.CreateEventFromGLsyncKHR;
        g_realDispatchTable.GetExtensionFunctionAddress = g_OpenCLModule.GetExtensionFunctionAddress;

        g_realDispatchTable._reservedForDeviceFissionEXT[0] = (void*)g_OpenCLModule.CreateSubDevicesEXT;
        g_realDispatchTable._reservedForDeviceFissionEXT[1] = (void*)g_OpenCLModule.RetainDeviceEXT;
        g_realDispatchTable._reservedForDeviceFissionEXT[2] = (void*)g_OpenCLModule.ReleaseDeviceEXT;
#ifdef _WIN32
        g_realDispatchTable._reservedForD3D10KHR[0] = (void*)g_OpenCLModule.GetDeviceIDsFromD3D10KHR;
        g_realDispatchTable._reservedForD3D10KHR[1] = (void*)g_OpenCLModule.CreateFromD3D10BufferKHR;
        g_realDispatchTable._reservedForD3D10KHR[2] = (void*)g_OpenCLModule.CreateFromD3D10Texture2DKHR;
        g_realDispatchTable._reservedForD3D10KHR[3] = (void*)g_OpenCLModule.CreateFromD3D10Texture3DKHR;
        g_realDispatchTable._reservedForD3D10KHR[4] = (void*)g_OpenCLModule.EnqueueAcquireD3D10ObjectsKHR;
        g_realDispatchTable._reservedForD3D10KHR[5] = (void*)g_OpenCLModule.EnqueueReleaseD3D10ObjectsKHR;
#endif
        // OpenCL 1.2
        g_realDispatchTable.CreateSubDevices = g_OpenCLModule.CreateSubDevices;
        g_realDispatchTable.RetainDevice = g_OpenCLModule.RetainDevice;
        g_realDispatchTable.ReleaseDevice = g_OpenCLModule.ReleaseDevice;
        g_realDispatchTable.CreateImage = g_OpenCLModule.CreateImage;
        g_realDispatchTable.CreateProgramWithBuiltInKernels = g_OpenCLModule.CreateProgramWithBuiltInKernels;
        g_realDispatchTable.CompileProgram = g_OpenCLModule.CompileProgram;
        g_realDispatchTable.LinkProgram = g_OpenCLModule.LinkProgram;
        g_realDispatchTable.UnloadPlatformCompiler = g_OpenCLModule.UnloadPlatformCompiler;
        g_realDispatchTable.GetKernelArgInfo = g_OpenCLModule.GetKernelArgInfo;
        g_realDispatchTable.EnqueueFillBuffer = g_OpenCLModule.EnqueueFillBuffer;
        g_realDispatchTable.EnqueueFillImage = g_OpenCLModule.EnqueueFillImage;
        g_realDispatchTable.EnqueueMigrateMemObjects = g_OpenCLModule.EnqueueMigrateMemObjects;
        g_realDispatchTable.EnqueueMarkerWithWaitList = g_OpenCLModule.EnqueueMarkerWithWaitList;
        g_realDispatchTable.EnqueueBarrierWithWaitList = g_OpenCLModule.EnqueueBarrierWithWaitList;
        g_realDispatchTable.GetExtensionFunctionAddressForPlatform = g_OpenCLModule.GetExtensionFunctionAddressForPlatform;
        g_realDispatchTable.CreateFromGLTexture = g_OpenCLModule.CreateFromGLTexture;

        g_realDispatchTable._reservedD3DExtensions[0] = g_OpenCLModule._reservedD3DExtensions[0];
        g_realDispatchTable._reservedD3DExtensions[1] = g_OpenCLModule._reservedD3DExtensions[1];
        g_realDispatchTable._reservedD3DExtensions[2] = g_OpenCLModule._reservedD3DExtensions[2];
        g_realDispatchTable._reservedD3DExtensions[3] = g_OpenCLModule._reservedD3DExtensions[3];
        g_realDispatchTable._reservedD3DExtensions[4] = g_OpenCLModule._reservedD3DExtensions[4];
        g_realDispatchTable._reservedD3DExtensions[5] = g_OpenCLModule._reservedD3DExtensions[5];
        g_realDispatchTable._reservedD3DExtensions[6] = g_OpenCLModule._reservedD3DExtensions[6];
        g_realDispatchTable._reservedD3DExtensions[7] = g_OpenCLModule._reservedD3DExtensions[7];
        g_realDispatchTable._reservedD3DExtensions[8] = g_OpenCLModule._reservedD3DExtensions[8];
        g_realDispatchTable._reservedD3DExtensions[9] = g_OpenCLModule._reservedD3DExtensions[9];

        g_realDispatchTable._reservedEGLExtensions[0] = g_OpenCLModule._reservedEGLExtensions[0];
        g_realDispatchTable._reservedEGLExtensions[1] = g_OpenCLModule._reservedEGLExtensions[1];
        g_realDispatchTable._reservedEGLExtensions[2] = g_OpenCLModule._reservedEGLExtensions[2];
        g_realDispatchTable._reservedEGLExtensions[3] = g_OpenCLModule._reservedEGLExtensions[3];

        g_realDispatchTable.CreateCommandQueueWithProperties = g_OpenCLModule.CreateCommandQueueWithProperties;
        g_realDispatchTable.CreatePipe = g_OpenCLModule.CreatePipe;
        g_realDispatchTable.GetPipeInfo = g_OpenCLModule.GetPipeInfo;
        g_realDispatchTable.SVMAlloc = g_OpenCLModule.SVMAlloc;
        g_realDispatchTable.SVMFree = g_OpenCLModule.SVMFree;
        g_realDispatchTable.EnqueueSVMFree = g_OpenCLModule.EnqueueSVMFree;
        g_realDispatchTable.EnqueueSVMMemcpy = g_OpenCLModule.EnqueueSVMMemcpy;
        g_realDispatchTable.EnqueueSVMMemFill = g_OpenCLModule.EnqueueSVMMemFill;
        g_realDispatchTable.EnqueueSVMMap = g_OpenCLModule.EnqueueSVMMap;
        g_realDispatchTable.EnqueueSVMUnmap = g_OpenCLModule.EnqueueSVMUnmap;
        g_realDispatchTable.CreateSamplerWithProperties = g_OpenCLModule.CreateSamplerWithProperties;
        g_realDispatchTable.SetKernelArgSVMPointer = g_OpenCLModule.SetKernelArgSVMPointer;
        g_realDispatchTable.SetKernelExecInfo = g_OpenCLModule.SetKernelExecInfo;

        // OpenCL 2.1
        g_realDispatchTable.clCloneKernel = g_OpenCLModule.CloneKernel;
        g_realDispatchTable.clEnqueueSVMMigrateMem = g_OpenCLModule.EnqueueSVMMigrateMem;
        g_realDispatchTable.clGetDeviceAndHostTimer = g_OpenCLModule.GetDeviceAndHostTimer;
        g_realDispatchTable.clGetHostTimer = g_OpenCLModule.GetHostTimer;
        g_realDispatchTable.clSetDefaultDeviceCommandQueue = g_OpenCLModule.SetDefaultDeviceCommandQueue;
        g_realDispatchTable.clGetKernelSubGroupInfo = g_OpenCLModule.GetKernelSubGroupInfo;
        g_realDispatchTable.clCreateProgramWithIL = g_OpenCLModule.CreateProgramWithIL;

        // OpenCL 2.2
        g_realDispatchTable.clSetProgramReleaseCallback = g_OpenCLModule.SetProgramReleaseCallback;
        g_realDispatchTable.clSetProgramSpecializationConstant = g_OpenCLModule.SetProgramSpecializationConstant;
    }
    else
    {
        // here, we are initializing the real function pointers with an agent's original dispatch table.
        SpAssertRet(nullptr != pRealTable);
        memcpy(&g_realDispatchTable, pRealTable, sizeof(g_realDispatchTable));
    }

    // Always use GetPlatformIDs from OpenCL ICD lib, as it is never assigned in the agent's dispatch table
    g_realDispatchTable.GetPlatformIDs = g_OpenCLModule.GetPlatformIDs;
}

CL_FUNC_TYPE InitExtensionFunction(const char* pFuncName, void* pFuncPtr)
{
    CL_FUNC_TYPE retVal = CL_FUNC_TYPE_Unknown;

    if (strcmp(pFuncName, "clSVMAllocAMD") == 0)
    {
        retVal = CL_FUNC_TYPE_clSVMAllocAMD;
        g_realExtensionFunctionTable.SVMAllocAMD = (clSVMAllocAMD_fn)pFuncPtr;
    }
    else if (strcmp(pFuncName, "clSVMFreeAMD") == 0)
    {
        retVal = CL_FUNC_TYPE_clSVMFreeAMD;
        g_realExtensionFunctionTable.SVMFreeAMD = (clSVMFreeAMD_fn)pFuncPtr;
    }
    else if (strcmp(pFuncName, "clEnqueueSVMFreeAMD") == 0)
    {
        retVal = CL_FUNC_TYPE_clEnqueueSVMFreeAMD;
        g_realExtensionFunctionTable.EnqueueSVMFreeAMD = (clEnqueueSVMFreeAMD_fn)pFuncPtr;
    }
    else if (strcmp(pFuncName, "clEnqueueSVMMemcpyAMD") == 0)
    {
        retVal = CL_FUNC_TYPE_clEnqueueSVMMemcpyAMD;
        g_realExtensionFunctionTable.EnqueueSVMMemcpyAMD = (clEnqueueSVMMemcpyAMD_fn)pFuncPtr;
    }
    else if (strcmp(pFuncName, "clEnqueueSVMMemFillAMD") == 0)
    {
        retVal = CL_FUNC_TYPE_clEnqueueSVMMemFillAMD;
        g_realExtensionFunctionTable.EnqueueSVMMemFillAMD = (clEnqueueSVMMemFillAMD_fn)pFuncPtr;
    }
    else if (strcmp(pFuncName, "clEnqueueSVMMapAMD") == 0)
    {
        retVal = CL_FUNC_TYPE_clEnqueueSVMMapAMD;
        g_realExtensionFunctionTable.EnqueueSVMMapAMD = (clEnqueueSVMMapAMD_fn)pFuncPtr;
    }
    else if (strcmp(pFuncName, "clEnqueueSVMUnmapAMD") == 0)
    {
        retVal = CL_FUNC_TYPE_clEnqueueSVMUnmapAMD;
        g_realExtensionFunctionTable.EnqueueSVMUnmapAMD = (clEnqueueSVMUnmapAMD_fn)pFuncPtr;
    }

    if (strcmp(pFuncName, "clSetKernelArgSVMPointerAMD") == 0)
    {
        retVal = CL_FUNC_TYPE_clSetKernelArgSVMPointerAMD;
        g_realExtensionFunctionTable.SetKernelArgSVMPointerAMD = (clSetKernelArgSVMPointerAMD_fn)pFuncPtr;
    }
    else if (strcmp(pFuncName, "clSetKernelExecInfoAMD") == 0)
    {
        retVal = CL_FUNC_TYPE_clSetKernelExecInfoAMD;
        g_realExtensionFunctionTable.SetKernelExecInfoAMD = (clSetKernelExecInfoAMD_fn)pFuncPtr;
    }
    else if (strcmp(pFuncName, "clCreateSsgFileObjectAMD") == 0)
    {
        retVal = CL_FUNC_TYPE_clCreateSsgFileObjectAMD;
        g_realExtensionFunctionTable.CreateSsgFileObjectAMD = (clCreateSsgFileObjectAMD_fn)pFuncPtr;
    }
    else if (strcmp(pFuncName, "clGetSsgFileObjectInfoAMD") == 0)
    {
        retVal = CL_FUNC_TYPE_clGetSsgFileObjectInfoAMD;
        g_realExtensionFunctionTable.GetSsgFileObjectInfoAMD = (clGetSsgFileObjectInfoAMD_fn)pFuncPtr;
    }
    else if (strcmp(pFuncName, "clRetainSsgFileObjectAMD") == 0)
    {
        retVal = CL_FUNC_TYPE_clRetainSsgFileObjectAMD;
        g_realExtensionFunctionTable.RetainSsgFileObjectAMD = (clRetainSsgFileObjectAMD_fn)pFuncPtr;
    }
    else if (strcmp(pFuncName, "clReleaseSsgFileObjectAMD") == 0)
    {
        retVal = CL_FUNC_TYPE_clReleaseSsgFileObjectAMD;
        g_realExtensionFunctionTable.ReleaseSsgFileObjectAMD = (clReleaseSsgFileObjectAMD_fn)pFuncPtr;
    }
    else if (strcmp(pFuncName, "clEnqueueReadSsgFileAMD") == 0)
    {
        retVal = CL_FUNC_TYPE_clEnqueueReadSsgFileAMD;
        g_realExtensionFunctionTable.EnqueueReadSsgFileAMD = (clEnqueueReadSsgFileAMD_fn)pFuncPtr;
    }
    else if (strcmp(pFuncName, "clEnqueueWriteSsgFileAMD") == 0)
    {
        retVal = CL_FUNC_TYPE_clEnqueueWriteSsgFileAMD;
        g_realExtensionFunctionTable.EnqueueWriteSsgFileAMD = (clEnqueueWriteSsgFileAMD_fn)pFuncPtr;
    }

    return retVal;
}

bool WriteDispatchTableToFile(cl_icd_dispatch_table* pTable)
{
    bool retVal = false;

    if (nullptr != pTable)
    {
        std::ofstream fout;

        fout.open(FileUtils::GetCLICDTableFile().c_str(), std::ios::binary);

        if (fout.good())
        {
            size_t tableSize = sizeof(cl_icd_dispatch_table);

            fout << tableSize;
            fout.write(reinterpret_cast<char*>(pTable), tableSize);

            retVal = true;
        }
    }

    return retVal;
}

bool ReadDispatchTableFromFile(cl_icd_dispatch_table* pTable)
{
    bool retVal = false;

    if (nullptr != pTable)
    {
        std::ifstream fin;

        fin.open(FileUtils::GetCLICDTableFile().c_str(), std::ios::binary);

        if (fin.good())
        {
            size_t tableSize = 0;
            fin >> tableSize;

            fin.read(reinterpret_cast<char*>(pTable), tableSize);

            retVal = true;
        }
    }

    return retVal;
}

cl_int InitAgent(cl_agent* pAgent,
                 const char* pAgentName,
                 cl_icd_dispatch_table* pICDDispatchTable,
                 cl_icd_dispatch_table* pRealDispatchTable)
{
    SpAssertRet(nullptr != pICDDispatchTable && nullptr != pRealDispatchTable) CL_INVALID_VALUE;

    bool isFirstAgent = IsFirstAgent(pAgentName);

    cl_icd_dispatch_table localTable;
    cl_int err = pAgent->GetICDDispatchTable(pAgent, &localTable, sizeof(localTable));

    if (CL_SUCCESS != err)
    {
        return err;
    }

    cl_icd_dispatch_table tableToCheckDevices;

    if (isFirstAgent)
    {
        memcpy(&tableToCheckDevices, &localTable, sizeof(localTable));
        bool success = WriteDispatchTableToFile(&localTable);

        if (!success)
        {
            GPULogger::Log(GPULogger::logERROR, "Unable to write dispatch table to file\n");
            return CL_INVALID_VALUE;
        }
    }
    else
    {
        bool success = ReadDispatchTableFromFile(&tableToCheckDevices);

        if (!success)
        {
            GPULogger::Log(GPULogger::logERROR, "Unable to read dispatch table from file\n");
            return CL_INVALID_VALUE;
        }
    }


    // using the local table, check for available devices. If there are none, this is not the right ICD
    cl_uint numDevices = 0;
    err = tableToCheckDevices.GetDeviceIDs(nullptr, CL_DEVICE_TYPE_ALL, 0, nullptr, &numDevices);

    if (CL_SUCCESS != err)
    {
        return err;
    }

    if (0 == numDevices)
    {
        return CL_DEVICE_NOT_FOUND;
    }

    memcpy(pICDDispatchTable, &localTable, sizeof(cl_icd_dispatch_table));
    memcpy(pRealDispatchTable, &tableToCheckDevices, sizeof(cl_icd_dispatch_table));

    return CL_SUCCESS;
}

bool IsFirstAgent(const std::string& thisModuleName)
{
    // We detect the first agent by looking at the CL_AGENT environment variable.
    // The first agent listed in the env var is the first agent loaded.

    SpAssertRet(!thisModuleName.empty()) false;

    // looking at the CL_AGENT environment variable. The first agent listed in the env var is the first agent loaded.
    std::string clAgentVar = OSUtils::Instance()->GetEnvVar(OCL_ENABLE_PROFILING_ENV_VAR);

    size_t thisModulePos = clAgentVar.find(thisModuleName);

    if (std::string::npos == thisModulePos)
    {
        GPULogger::Log(GPULogger::logERROR, "Unexpected module %s\n", thisModuleName.c_str());
    }

    size_t anyModuleMinPos = std::string::npos;

    anyModuleMinPos = std::min(anyModuleMinPos, clAgentVar.find(CL_TRACE_AGENT_DLL));
    anyModuleMinPos = std::min(anyModuleMinPos, clAgentVar.find(CL_SUB_KERNEL_PROFILE_AGENT_DLL));
    anyModuleMinPos = std::min(anyModuleMinPos, clAgentVar.find(CL_PROFILE_AGENT_DLL));
    anyModuleMinPos = std::min(anyModuleMinPos, clAgentVar.find(CL_OCCUPANCY_AGENT_DLL));
    anyModuleMinPos = std::min(anyModuleMinPos, clAgentVar.find(CL_THREAD_TRACE_AGENT_DLL));

    // If we are the first agent, we serialize the original dispatch table to disk.
    // If we are not the first agent, we read the previously-serialized table from disk.
    bool result = (thisModulePos != std::string::npos) && (thisModulePos <= anyModuleMinPos);

    return result;
}
