# UpdateCommonMap.py
#
#    Map of GitHub project names to clone target paths, relative to the RCP
#    project root on the local disk.

# GitHub GPUOpen-Tools projects map

GitHubMapping = {
    "common-lib-amd-ADL"                     : "../Common/Lib/AMD/ADL",
    "common-lib-AMD-ACL"                     : "../Common/Lib/AMD/ACL",
    "common-lib-amd-APPSDK-3.0"              : "../Common/Lib/AMD/APPSDK",
    "common-lib-ext-Boost-1.59"              : "../Common/Lib/Ext/Boost",
    "common-lib-ext-mhook"                   : "../Common/Lib/Ext/mhook",
    "common-lib-ext-tinyxml-2.6.2"           : "../Common/Lib/Ext/tinyxml",
    "common-lib-ext-utf8cpp"                 : "../Common/Lib/Ext/utf8cpp",
    "common-src-ACLModuleManager"            : "../Common/Src/ACLModuleManager",
    "common-src-ADLUtil"                     : "../Common/Src/ADLUtil",
    "common-src-AMDTActivityLogger"          : "../Common/Src/AMDTActivityLogger",
    "common-src-AMDTApplication"             : "../Common/Src/AMDTApplication",
    "common-src-AMDTBaseTools"               : "../Common/Src/AMDTBaseTools",
    "common-src-AMDTInterceptor"             : "../Common/Src/AMDTInterceptor",
    "common-src-AMDTOSWrappers"              : "../Common/Src/AMDTOSWrappers",
    "common-src-DeviceInfo"                  : "../Common/Src/DeviceInfo",
    "common-src-DynamicLibraryModule"        : "../Common/Src/DynamicLibraryModule",
    "common-src-HSAUtils"                    : "../Common/Src/HSAUtils",
    "common-src-Miniz"                       : "../Common/Src/Miniz",
    "common-src-SCons"                       : "../Common/Src/SCons",
    "common-src-TSingleton"                  : "../Common/Src/TSingleton",
    "common-src-Vsprops"                     : "../Common/Src/Vsprops",
    "common-src-VersionInfo"                 : "../Common/Src/VersionInfo",
}

downloadMappingWin = {
    "https://github.com/GPUOpen-Tools/GPA/releases/download/v3.2/GPUPerfAPI-3.2.623.0.zip" : "../Common/Lib/AMD/GPUPerfAPI",
    "https://github.com/GPUOpen-Tools/RCP/releases/download/v5.4/RadeonComputeProfiler-v5.4.6937.zip" : "../Common/Lib/AMD/VKStableClocks"
}

downloadMappingLin = {
    "https://github.com/GPUOpen-Tools/GPA/releases/download/v3.2/GPUPerfAPI.3.2.858-lnx.tgz" : "../Common/Lib/AMD/GPUPerfAPI",
    "https://github.com/GPUOpen-Tools/RCP/releases/download/v5.4/RadeonComputeProfiler-v5.4.6906.tgz" : "../Common/Lib/AMD/VKStableClocks"
}

# FileToCopy,  [BaseDir, SubDir, SrcPath]
vkStableClocksMappingWin = {
    "VkStableClocks.exe" : ["../Common/Lib/AMD/VKStableClocks", "VKStableClocks", "RadeonComputeProfiler/bin"],
    "VKStableClocks-x64.exe" : ["../Common/Lib/AMD/VKStableClocks", "VKStableClocks", "RadeonComputeProfiler/bin"],
}

vkStableClocksMappingLin = {
    "VkStableClocks" : ["../Common/Lib/AMD/VKStableClocks", "VKStableClocks", "bin"],
}

