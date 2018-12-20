# UpdateCommonMap.py
#

# zip or tgz files to download
downloadMappingWin = {
    "https://github.com/GPUOpen-Tools/GPA/releases/download/v3.3/GPUPerfAPI-3.3.799.zip" : "../Common/Lib/AMD/GPUPerfAPI",
    "https://github.com/GPUOpen-Tools/RCP/releases/download/v5.5/RadeonComputeProfiler-v5.5.6980.zip" : "../Common/Lib/AMD/VKStableClocks"
}

downloadMappingLin = {
    "https://github.com/GPUOpen-Tools/GPA/releases/download/v3.3/GPUPerfAPI-3.3.1078.tgz" : "../Common/Lib/AMD/GPUPerfAPI",
    "https://github.com/GPUOpen-Tools/RCP/releases/download/v5.5/RadeonComputeProfiler-v5.5.6947.tgz" : "../Common/Lib/AMD/VKStableClocks"
}

# FileToCopy,  [BaseDir, SubDir, SrcPath]
vkStableClocksMappingWin = {
    "VkStableClocks.exe" : ["../Common/Lib/AMD/VKStableClocks", "VKStableClocks", "RadeonComputeProfiler/bin"],
    "VKStableClocks-x64.exe" : ["../Common/Lib/AMD/VKStableClocks", "VKStableClocks", "RadeonComputeProfiler/bin"],
}

vkStableClocksMappingLin = {
    "VkStableClocks" : ["../Common/Lib/AMD/VKStableClocks", "VKStableClocks", "bin"],
}

