//==============================================================================
// Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief This class manages the retrieval of HSA kernel source, IL,
///        ISA from the HSA run-time.
//==============================================================================

#include <vector>
#include <string>
#include <algorithm>
#include <cstdlib>

#include "HSAKernelAssembly.h"
#include "ComgrUtils.h"
#include "../Common/StringUtils.h"
#include "../Common/FileUtils.h"
#include "../Common/GlobalSettings.h"
#include "../Common/Logger.h"

HsaKernelAssembly::HsaKernelAssembly() : m_shouldOutputIsa(false), m_shouldOutputHsail(false)
{
};

void HsaKernelAssembly::SetOutputIsaFlag(const bool& shouldOutputIsa)
{
    m_shouldOutputIsa = shouldOutputIsa;
}

void HsaKernelAssembly::SetOutputHsailFlag(const bool& shouldOuputHsail)
{
    m_shouldOutputHsail = shouldOuputHsail;
}

bool HsaKernelAssembly::Generate(const std::vector<char>& codeObjectBinary,
                                 const std::string&       deviceName,
                                 const std::string&       kernelName,
                                 const std::string&       agentName,
                                 const std::string&       outputDir,
                                 const bool&              isGPU)
{
    bool kernelFilesGenerated = false;

    kernelFilesGenerated = GenerateAssemblyFiles(codeObjectBinary, deviceName, kernelName, agentName, outputDir, isGPU);

    if (!kernelFilesGenerated)
    {
        GPULogger::Log(GPULogger::logWARNING, "Unable to generate kernel using COMGR Module\n");
    }

    return true;
}

bool HsaKernelAssembly::GenerateAssemblyFiles(const std::vector<char>& codeObjectBinary,
                                              const std::string&       deviceName,
                                              const std::string&       kernelName,
                                              const std::string&       agentName,
                                              const std::string&       outputDir,
                                              const bool&              isGPU)
{
    bool ret = false;
    bool assemblyGenerated = false;

    // Current COMGR supports output ISA only
    if (m_shouldOutputIsa)
    {
        if (isGPU)
        {
            if (AMDT::ComgrEntryPoints::Instance()->EntryPointsValid())
            {
                std::vector<char> assemblyBuffer;

                auto codeObj = AMDT::CodeObj::OpenBuffer(codeObjectBinary);

                if (nullptr != codeObj)
                {
                    std::string options("");

                    if (GenerateCodeObjectTargetString(deviceName, options))
                    {
                        assemblyGenerated = codeObj->ExtractAssemblyData(assemblyBuffer, options);
                    }
                }

                if (assemblyGenerated)
                {
                    std::string outputFileFullPath = outputDir + kernelName + "_" + agentName + "_" + deviceName + ".isa";
                    std::string assemblyBufferString(assemblyBuffer.begin(), assemblyBuffer.end());
                    ret = FileUtils::WriteFile(outputFileFullPath, assemblyBufferString);
                }
            }

            if (!assemblyGenerated)
            {
                GPULogger::Log(GPULogger::logWARNING, "Unable to use comgr library to extract ISA\n");
            }
        }
    }

    return ret;
}

bool HsaKernelAssembly::GenerateCodeObjectTargetString(const std::string& deviceName,
                                                       std::string&       codeObjectTargetString)
{
    codeObjectTargetString = "";
    std::string arch("amdgcn");
    std::string vendor("amd");
    std::string os("amdhsa");
    // The env should be set empty by default.
    std::string env("");
    std::string processor(AMDTDeviceInfoUtils::Instance()->TranslateDeviceName(deviceName.c_str()));

    // TODO: The xnack support is currently not available in COMGR library.
    // See https://llvm.org/docs/AMDGPUUsage.html#amdgpu-target-feature-table,
    // for the XNACK support list of hardware.
    // std::string targetFeatures("");
    // if ("gfx801" == processor ||
    //     "gfx810" == processor ||
    //     "gfx902" == processor)
    // {
    //     targetFeatures = "xnack";
    // }
    // else
    // {
    //     targetFeatures = "no-xnack";
    // }

    codeObjectTargetString = arch + "-" + vendor + "-" + os + "-" + env + "-" + processor;

    return true;
}
