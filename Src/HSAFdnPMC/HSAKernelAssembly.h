//==============================================================================
// Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief This class manages the retrieval of HSAIL, ISA
///        from the HSA run-time.
//==============================================================================

#ifndef _HSA_KERNEL_ASSEMBLY_H_
#define _HSA_KERNEL_ASSEMBLY_H_

#include <string>
#include <vector>

#include "DeviceInfoUtils.h"

/// This class manages the assembly files (HSAIL/ISA/x86/source) generation and output.
class HsaKernelAssembly
{
public:
    /// Constructor.
    HsaKernelAssembly();

    /// Destructor.
    ~HsaKernelAssembly() = default;

    /// Generate the kernel assembly.
    /// \param codeObjectBinary the binary representation of code object.
    /// \param deviceName the device name.
    /// \param kernelName the kernel name.
    /// \param agentName the agent name.
    /// \param outputDir the output directory.
    /// \param isGPU Is kernel compiled for GPU.
    /// \return true if successful, false otherwise.
    bool Generate(const std::vector<char>& codeObjectBinary,
                  const std::string&       deviceName,
                  const std::string&       kernelName,
                  const std::string&       agentName,
                  const std::string&       outputDir,
                  const bool&              isGPU);

    /// Generate the kernel assembly files.
    /// \param codeObjectBinary the binary representation of code object.
    /// \param deviceName the device name.
    /// \param kernelName the kernel name.
    /// \param agentName the agent name.
    /// \param outputDir the output directory.
    /// \param isGPU Is kernel compiled for GPU.
    /// \return true if successful, false otherwise.
    bool GenerateAssemblyFiles(const std::vector<char>& codeObjectBinary,
                               const std::string&       deviceName,
                               const std::string&       kernelName,
                               const std::string&       agentName,
                               const std::string&       outputDir,
                               const bool&              isGPU);

    /// Set the flag indicating if the ISA file should be written out.
    /// \param shouldOutputIsa flag indicating if the ISA file should be written out.
    void SetOutputIsaFlag(const bool& shouldOutputIsa);

    /// sets the flag indicating if the HSAIL file should be written out.
    /// \param shouldOuputHsail flag indicating if the HSAIL file should be written out.
    void SetOutputHsailFlag(const bool& shouldOuputHsail);

    /// Generate the code object target string.
    /// The naming convention should follow the LLVM documentation.
    /// https://llvm.org/docs/AMDGPUUsage.html#code-object-target-identification
    /// \param deviceName the device name.
    /// \param codeObjectTargetString the generated code object target string.
    /// \return true if successful, false otherwise.
    bool GenerateCodeObjectTargetString(const std::string& deviceName,
                                        std::string&       codeObjectTargetString);

private:

    bool m_shouldOutputIsa;   ///< flag indicating whether or not to write out the ISA file.
    bool m_shouldOutputHsail; ///< flag indicating whether or not to write out the HSAIL file.
};

#endif
