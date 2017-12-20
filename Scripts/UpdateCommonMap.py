# UpdateCommonMap.py
#
#    Map of GitHub project names to clone target paths, relative to the RCP
#    project root on the local disk.

# GitHub GPUOpen-Tools projects map

GitHubMapping = {
    "common-lib-amd-ADL"                     : ["../Common/Lib/AMD/ADL", "99cb60efe22718c1e12415b6b381facfdc7c7979"],
    "common-lib-AMD-ACL"                     : ["../Common/Lib/AMD/ACL", "db0c78be0d38e856ba2219da124b271975c7b5ea"],
    "common-lib-amd-APPSDK-3.0"              : ["../Common/Lib/AMD/APPSDK", "1a091274ef40a85a5d973ef9d5b687bb970130f2"],
    "common-lib-AMD-CAL-8.95"                : ["../Common/Lib/AMD/CAL", "d31406a20330deeefa3acb8191e2b865b0af45eb"],
    "common-lib-ext-Boost-1.59"              : ["../Common/Lib/Ext/Boost", "d50f858fdff41c549595bb1c84d236206d117772"],
    "common-lib-ext-mhook"                   : ["../Common/Lib/Ext/mhook", "9a595f45dfc39dcaede3b44a5af30e85e77b770e"],
    "common-lib-ext-tinyxml-2.6.2"           : ["../Common/Lib/Ext/tinyxml", "0ed29c3884888228dc923a4a176098eedf0a6277"],
    "common-lib-ext-utf8cpp"                 : ["../Common/Lib/Ext/utf8cpp", "53048f8f7eb69f315a957637d0422845c6652fe5"],
    "common-lib-ext-zlib-1.2.8"              : ["../Common/Lib/Ext/zlib", "59eddd52fe972df9e09c903bfe99ece3262f9f99"],
    "common-src-ACLModuleManager"            : ["../Common/Src/ACLModuleManager", "26bcd9a586f60eabf45f5962708921a49859a764"],
    "common-src-ADLUtil"                     : ["../Common/Src/ADLUtil", "fa6949339b23529b30cf186188039adcb63ec596"],
    "common-src-AMDTActivityLogger"          : ["../Common/Src/AMDTActivityLogger", "c799c5d32a0fc4f3a36fe2b31a377e34d4323fb5"],
    "common-src-AMDTApplication"             : ["../Common/Src/AMDTApplication", "c7fec873a2f804980f38f44fed909afd8600c832"],
    "common-src-AMDTBaseTools"               : ["../Common/Src/AMDTBaseTools", "853f0450c4f1b8367baae81fdb603c0f2cc84a19"],
    "common-src-AMDTInterceptor"             : ["../Common/Src/AMDTInterceptor", "789ab6ccfa4b603d1da25ab0a77be5361813cba9"],
    "common-src-AMDTOSWrappers"              : ["../Common/Src/AMDTOSWrappers", "7e83f9ee4959ea427cbe0d57a8c794e152822ead"],
    "common-src-CElf"                        : ["../Common/Src/CElf", "24477296cdc5ecddc4c62ebe1b0af6346ec4b19a"],
    "common-src-DeviceInfo"                  : ["../Common/Src/DeviceInfo", "9889ad75f5d79a2453e541d76302d13858d11655"],
    "common-src-DynamicLibraryModule"        : ["../Common/Src/DynamicLibraryModule", "c331489a4167aca1f6d2e9a907ab7758c0b9bd90"],
    "common-src-GPUPerfAPIUtils"             : ["../Common/Src/GPUPerfAPIUtils", "d10457ebd4846a84820f83a7dd3e2e11ec1be081"],
    "common-src-HSAUtils"                    : ["../Common/Src/HSAUtils", "370768ab279768efc51520da98ea71148a90b27a"],
    "common-src-Miniz"                       : ["../Common/Src/Miniz", "a958cde31565769681aa3d7934c3d38c52940f4e"],
    "common-src-SCons"                       : ["../Common/Src/SCons", "7bea261af8ef079ee5a050fbce820f668d537b30"],
    "common-src-TSingleton"                  : ["../Common/Src/TSingleton", "2a338d5b7e649ffba0225772c521726c18b13fff"],
    "common-src-Vsprops"                     : ["../Common/Src/Vsprops", "5a434b269727ed1f36c3c31aa56a0ca06502b198"],
    "common-src-VersionInfo"                 : ["../Common/Src/VersionInfo", "e82cca94cc287a4ac53dc6e82f3525e31de80e05"],
}

downloadMappingWin = {
    "https://github.com/GPUOpen-Tools/GPA/releases/download/v2.23.1/GPUPerfAPI-2.23.2392.1.zip" : "../Common/Lib/AMD/GPUPerfAPI",
}

downloadMappingLin = {
    "https://github.com/GPUOpen-Tools/GPA/releases/download/v2.23.1/GPUPerfAPI.2.23.1986-lnx.tgz" : "../Common/Lib/AMD/GPUPerfAPI",
}


