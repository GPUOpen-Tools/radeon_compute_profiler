//==============================================================================
// Copyright (c) 2016 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  This file provides kernel name demangling helper function
//==============================================================================

#include "HSAKernelDemangler.h"

#include <sstream>
#include <fstream>
#include <unordered_map>
#include <mutex>
#include "Logger.h"
#include "GlobalSettings.h"
#include <cstdio>
#include "FileUtils.h"
#include "Defs.h"
#include "StringUtils.h"


static std::unordered_map<std::string, std::string> gs_demangledKernelsMap;
static std::mutex gs_demanglerMutex;

std::string DemangleKernelName(std::string& kernelName)
{
    std::lock_guard<std::mutex> lock(gs_demanglerMutex);
    std::string demangledKernelName;

    if (gs_demangledKernelsMap.count(kernelName) > 0)
    {
        demangledKernelName = gs_demangledKernelsMap[kernelName];
    }
    else
    {
        demangledKernelName = kernelName;

        if (!GlobalSettings::GetInstance()->m_params.m_bDisableKernelDemangling && !system("which c++filt > /dev/null 2>&1"))
        {
            std::string tempMangledKernelNameFile = FileUtils::GetDefaultOutputPath() + "/rcp_mangled_kernel";
            std::string tempDemangledKernelNameFile = FileUtils::GetDefaultOutputPath() + "/rcp_demangled_kernel";

            if ('Z' == demangledKernelName.at(0))
            {
                demangledKernelName.insert(demangledKernelName.begin(), '_');
            }

            std::ofstream fout(tempMangledKernelNameFile);

            if (fout.is_open())
            {
                fout << demangledKernelName;
            }

            fout.close();

            std::stringstream demanglingCommand;
            demanglingCommand << "c++filt -p" << " < " << tempMangledKernelNameFile << " > " << tempDemangledKernelNameFile;
            int retCode = system(demanglingCommand.str().c_str());

            if (retCode != 0)
            {
                GPULogger::Log(GPULogger::logERROR, "Unable to demangle the kernel Name\n");
            }

            std::ifstream fin(tempDemangledKernelNameFile);

            if (fin.is_open())
            {
                getline(fin, demangledKernelName);
            }

            fin.close();

#ifndef _DEBUG
            bool success = std::remove(tempMangledKernelNameFile.c_str()) == 0 ? true : false;
            success &= std::remove(tempDemangledKernelNameFile.c_str()) == 0 ? true : false;

            if (!success)
            {
                GPULogger::Log(GPULogger::logERROR, "Unable to delete the temporary files %s %s", tempMangledKernelNameFile.c_str(), tempDemangledKernelNameFile.c_str());
            }

#endif

        }

        demangledKernelName = StringUtils::Replace(demangledKernelName, " ", std::string(SPACE));
        demangledKernelName = StringUtils::Replace(demangledKernelName, ",", std::string(COMMA));

        gs_demangledKernelsMap[kernelName] = demangledKernelName;
    }

    return demangledKernelName;
}
