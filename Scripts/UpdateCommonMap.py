# UpdateCommonMap.py
#
#    Map of GitHub project names to clone target paths, relative to the RCP
#    project root on the local disk.

# GitHub GPUOpen-Tools projects map

GitHubMapping = {
    "common-lib-amd-ADL"                     : ["../Common/Lib/AMD/ADL", "04b97aa3138c282d9c2b84b3b2f3d06decbea6df"],
    "common-lib-AMD-ACL"                     : ["../Common/Lib/AMD/ACL", "f46a67b67b1bd774441d1a3ad46e8fa38e371a16"],
    "common-lib-amd-APPSDK-3.0"              : ["../Common/Lib/AMD/APPSDK", "4e6d30e406d2e5a65e1d65e404fe6df5f772a32b"],
    "common-lib-AMD-CAL-8.95"                : ["../Common/Lib/AMD/CAL", "0b2a4bf4d5e3d090879b0473416d35a0d6dba1c3"],
    "common-lib-ext-Boost-1.59"              : ["../Common/Lib/Ext/Boost", "d50f858fdff41c549595bb1c84d236206d117772"],
    "common-lib-ext-mhook"                   : ["../Common/Lib/Ext/mhook", "9a595f45dfc39dcaede3b44a5af30e85e77b770e"],
    "common-lib-ext-tinyxml-2.6.2"           : ["../Common/Lib/Ext/tinyxml", "0ed29c3884888228dc923a4a176098eedf0a6277"],
    "common-lib-ext-utf8cpp"                 : ["../Common/Lib/Ext/utf8cpp", "53048f8f7eb69f315a957637d0422845c6652fe5"],
    "common-src-ACLModuleManager"            : ["../Common/Src/ACLModuleManager", "5322ca3a65dceecb033bff3dcc7fa2d344274eef"],
    "common-src-ADLUtil"                     : ["../Common/Src/ADLUtil", "2f57a3e44067379a1bdbb7b420d5e95ab2cab55c"],
    "common-src-AMDTActivityLogger"          : ["../Common/Src/AMDTActivityLogger", "0f48f16a52c00f86796dd19cc376a533de674875"],
    "common-src-AMDTApplication"             : ["../Common/Src/AMDTApplication", "3fe9d7352e106bdaba860203e70f6903fb4b36c5"],
    "common-src-AMDTBaseTools"               : ["../Common/Src/AMDTBaseTools", "8ab67ae9ab00cb13d6c9867dbc4fe80bd2776045"],
    "common-src-AMDTInterceptor"             : ["../Common/Src/AMDTInterceptor", "49e9f0ffa192df392662d1b739e0959ee7632b1a"],
    "common-src-AMDTOSWrappers"              : ["../Common/Src/AMDTOSWrappers", "fc8be6e3a26497884b713eab5bb8345f08cba64c"],
    "common-src-CElf"                        : ["../Common/Src/CElf", "21e22d1f3d97124226f60e643d4a954749685ccb"],
    "common-src-DeviceInfo"                  : ["../Common/Src/DeviceInfo", "10fd4de85f62d9146480e2e76900f284d58dcecd"],
    "common-src-DynamicLibraryModule"        : ["../Common/Src/DynamicLibraryModule", "37160c2c7543f08b04cf0ac633a38f221bfad31e"],
    "common-src-HSAUtils"                    : ["../Common/Src/HSAUtils", "15430f9c5188686c9b1b8292f21d791380c0e2f7"],
    "common-src-Miniz"                       : ["../Common/Src/Miniz", "a958cde31565769681aa3d7934c3d38c52940f4e"],
    "common-src-SCons"                       : ["../Common/Src/SCons", "fa10c92850242f13400422d44fe5512f016900db"],
    "common-src-TSingleton"                  : ["../Common/Src/TSingleton", "ebde730c07eac1c1da7f486d65517b93e1550edb"],
    "common-src-Vsprops"                     : ["../Common/Src/Vsprops", "69c97737d688a2e3fe5a5d4ba844ae6ab1cd35c4"],
    "common-src-VersionInfo"                 : ["../Common/Src/VersionInfo", "1f66f52bf900821e002578f06ed78d53faf2268d"],
}

downloadMappingWin = {
    "https://github.com/GPUOpen-Tools/GPA/releases/download/v3.1a/GPUPerfAPI-3.1.571.0.zip" : "../Common/Lib/AMD/GPUPerfAPI",
    "https://github.com/GPUOpen-Tools/RCP/releases/download/v5.4/RadeonComputeProfiler-v5.4.6937.zip" : "../Common/Lib/AMD/VKStableClocks"
}

downloadMappingLin = {
    "https://github.com/GPUOpen-Tools/GPA/releases/download/v3.1a/GPUPerfAPI.3.1.793-lnx.tgz" : "../Common/Lib/AMD/GPUPerfAPI",
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

