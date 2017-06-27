# UpdateCommonMap.py
#
#    Map of GitHub project names to clone target paths, relative to the RCP
#    project root on the local disk.

# GitHub GPUOpen-Tools projects map

GitHubMapping = {
    "common-lib-amd-ADL"                     : ["../Common/Lib/AMD/ADL", "0f3122ba840b193ca9ebd8f050a7ddac58f3b27e"],
    "common-lib-AMD-ACL"                     : ["../Common/Lib/AMD/ACL", "db0c78be0d38e856ba2219da124b271975c7b5ea"],
    "common-lib-amd-APPSDK-3.0"              : ["../Common/Lib/AMD/APPSDK", "acb9bb08f23e5653bbe8b6021a2cf89f1c7fd26f"],
    "common-lib-AMD-CAL-8.95"                : ["../Common/Lib/AMD/CAL", "d31406a20330deeefa3acb8191e2b865b0af45eb"],
    "common-lib-ext-Boost-1.59"              : ["../Common/Lib/Ext/Boost", "d50f858fdff41c549595bb1c84d236206d117772"],
    "common-lib-ext-mhook"                   : ["../Common/Lib/Ext/mhook", "9a595f45dfc39dcaede3b44a5af30e85e77b770e"],
    "common-lib-ext-tinyxml-2.6.2"           : ["../Common/Lib/Ext/tinyxml", "0ed29c3884888228dc923a4a176098eedf0a6277"],
    "common-lib-ext-utf8cpp"                 : ["../Common/Lib/Ext/utf8cpp", "53048f8f7eb69f315a957637d0422845c6652fe5"],
    "common-lib-ext-zlib-1.2.8"              : ["../Common/Lib/Ext/zlib", "59eddd52fe972df9e09c903bfe99ece3262f9f99"],
    "common-src-ACLModuleManager"            : ["../Common/Src/ACLModuleManager", "b1d5a9353e20e35ea316761cdd59ba0092ef780b"],
    "common-src-ADLUtil"                     : ["../Common/Src/ADLUtil", "5a54c478c06ced583ac8ad74228f3e84f90e44f6"],
    "common-src-AMDTActivityLogger"          : ["../Common/Src/AMDTActivityLogger", "a2400368226b680bbd20629bde19c50798b7b4b0"],
    "common-src-AMDTApplication"             : ["../Common/Src/AMDTApplication", "c7fec873a2f804980f38f44fed909afd8600c832"],
    "common-src-AMDTBaseTools"               : ["../Common/Src/AMDTBaseTools", "853f0450c4f1b8367baae81fdb603c0f2cc84a19"],
    "common-src-AMDTInterceptor"             : ["../Common/Src/AMDTInterceptor", "789ab6ccfa4b603d1da25ab0a77be5361813cba9"],
    "common-src-AMDTOSWrappers"              : ["../Common/Src/AMDTOSWrappers", "627ca9fdcd866ba78d8c737b95fbe75ec2678d00"],
    "common-src-AMDTMutex"                   : ["../Common/Src/AMDTMutex", "fb68811ed3a475cc9ab1c5d5e0482658ec679716"],
    "common-src-CElf"                        : ["../Common/Src/CElf", "24477296cdc5ecddc4c62ebe1b0af6346ec4b19a"],
    "common-src-DeviceInfo"                  : ["../Common/Src/DeviceInfo", "5bc6e3ca77254ca2b42e57e7af4260170f9952a2"],
    "common-src-DynamicLibraryModule"        : ["../Common/Src/DynamicLibraryModule", "4b1587a50119c0a59ce3472facbaa2af7c243fdf"],
    "common-src-GPUPerfAPIUtils"             : ["../Common/Src/GPUPerfAPIUtils", "d10457ebd4846a84820f83a7dd3e2e11ec1be081"],
    "common-src-HSAUtils"                    : ["../Common/Src/HSAUtils", "370768ab279768efc51520da98ea71148a90b27a"],
    "common-src-Miniz"                       : ["../Common/Src/Miniz", "a958cde31565769681aa3d7934c3d38c52940f4e"],
    "common-src-SCons"                       : ["../Common/Src/SCons", "ef057605c47b3504f18c66382832099a37c1793b"],
    "common-src-TSingleton"                  : ["../Common/Src/TSingleton", "2a338d5b7e649ffba0225772c521726c18b13fff"],
    "common-src-Vsprops"                     : ["../Common/Src/Vsprops", "5a434b269727ed1f36c3c31aa56a0ca06502b198"],
    "common-src-VersionInfo"                 : ["../Common/Src/VersionInfo", "d5690aefe5107252d287d73fc6008b8ceaf3cdf0"],
}

downloadMappingWin = {
    "https://github.com/GPUOpen-Tools/GPA/releases/download/v2.23/GPUPerfAPI-2.23.2382.0.zip" : "../Common/Lib/AMD/GPUPerfAPI",
}

downloadMappingLin = {
    "https://github.com/GPUOpen-Tools/GPA/releases/download/v2.23/GPUPerfAPI.2.23.1973-lnx.tgz" : "../Common/Lib/AMD/GPUPerfAPI",
}


