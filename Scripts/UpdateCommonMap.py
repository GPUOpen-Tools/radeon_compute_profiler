# UpdateCommonMap.py
#
#    Map of GitHub project names to clone target paths, relative to the RCP
#    project root on the local disk.

# GitHub GPUOpen-Tools projects map

GitHubMapping = {
    "common-lib-amd-ADL"                     : "../Common/Lib/AMD/ADL",
    "common-lib-AMD-ACL"                     : "../Common/Lib/AMD/ACL",
    "common-lib-amd-APPSDK-3.0"              : "../Common/Lib/AMD/APPSDK",
    "common-lib-AMD-CAL-8.95"                : "../Common/Lib/AMD/CAL",
    "common-lib-ext-Boost-1.59"              : "../Common/Lib/Ext/Boost",
    "common-lib-ext-mhook"                   : "../Common/Lib/Ext/mhook",
    "common-lib-ext-tinyxml-2.6.2"           : "../Common/Lib/Ext/tinyxml",
    "common-lib-ext-utf8cpp"                 : "../Common/Lib/Ext/utf8cpp",
    "common-lib-ext-zlib-1.2.8"              : "../Common/Lib/Ext/zlib",
    "common-src-ACLModuleManager"            : "../Common/Src/ACLModuleManager",
    "common-src-ADLUtil"                     : "../Common/Src/ADLUtil",
    "common-src-AMDTActivityLogger"          : "../Common/Src/AMDTActivityLogger",
    "common-src-AMDTApplication"             : "../Common/Src/AMDTApplication",
    "common-src-AMDTBaseTools"               : "../Common/Src/AMDTBaseTools",
    "common-src-AMDTInterceptor"             : "../Common/Src/AMDTInterceptor",
    "common-src-AMDTOSWrappers"              : "../Common/Src/AMDTOSWrappers",
    "common-src-AMDTMutex"                   : "../Common/Src/AMDTMutex",
    "common-src-CElf"                        : "../Common/Src/CElf",
    "common-src-DeviceInfo"                  : "../Common/Src/DeviceInfo",
    "common-src-DynamicLibraryModule"        : "../Common/Src/DynamicLibraryModule",
    "common-src-GPUPerfAPIUtils"             : "../Common/Src/GPUPerfAPIUtils",
    "common-src-HSAUtils"                    : "../Common/Src/HSAUtils",
    "common-src-Miniz"                       : "../Common/Src/Miniz",
    "common-src-SCons"                       : "../Common/Src/SCons",
    "common-src-TSingleton"                  : "../Common/Src/TSingleton",
    "common-src-Vsprops"                     : "../Common/Src/Vsprops",
    "common-src-VersionInfo"                 : "../Common/Src/VersionInfo",
}

downloadMappingWin = {
    "https://github.com/GPUOpen-Tools/GPA/releases/download/v2.23.1/GPUPerfAPI-2.23.2392.1.zip" : "../Common/Lib/AMD/GPUPerfAPI",
}

downloadMappingLin = {
    "https://github.com/GPUOpen-Tools/GPA/releases/download/v2.23.1/GPUPerfAPI.2.23.1986-lnx.tgz" : "../Common/Lib/AMD/GPUPerfAPI",
}


