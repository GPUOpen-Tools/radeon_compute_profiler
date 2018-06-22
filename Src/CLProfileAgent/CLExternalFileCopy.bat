: %1 = path to the GPUPerfAPI directory ($(CommonDir)\Lib\AMD\GPUPerfAPI\X_Y)
: %2 = path to the VkStableClocks directory ($(CommonDir)\Lib\AMD\VkStableClocks\VkStableClocks)
: %3 = AMDTBuildSuffix
: %4 = AMDTPlatform
: %5 = AMDTPlatformSuffix
: %6 = Destination directory for build
: %7 = Optional Destination directory for build
: %8 = Optional Destination directory for build

: There is no NDA version of GPUPerfAPI, so map NDA to Public version
set BUILDSUFFIX=%3
IF [%BUILDSUFFIX%] == ["-nda"] set BUILDSUFFIX=

IF EXIST %1\Bin%BUILDSUFFIX%\%4\GPUPerfAPICL%5%BUILDSUFFIX%.dll ECHO Copying "GPUPerfAPICL%5%BUILDSUFFIX%.dll" into build directory
IF EXIST %1\Bin%BUILDSUFFIX%\%4\GPUPerfAPICL%5%BUILDSUFFIX%.dll COPY %1\Bin%BUILDSUFFIX%\%4\GPUPerfAPICL%5%BUILDSUFFIX%.dll %6
IF [%7] == [] GOTO COUNTERS
IF NOT EXIST %7 MKDIR %7
IF EXIST %1\Bin%BUILDSUFFIX%\%4\GPUPerfAPICL%5%BUILDSUFFIX%.dll COPY %1\Bin%BUILDSUFFIX%\%4\GPUPerfAPICL%5%BUILDSUFFIX%.dll %7
IF [%8] == [] GOTO COUNTERS
IF NOT EXIST %8 MKDIR %8
IF EXIST %1\Bin%BUILDSUFFIX%\%4\GPUPerfAPICL%5%BUILDSUFFIX%.dll COPY %1\Bin%BUILDSUFFIX%\%4\GPUPerfAPICL%5%BUILDSUFFIX%.dll %8

:COUNTERS

IF EXIST %1\Bin%BUILDSUFFIX%\%4\GPUPerfAPICounters%5%BUILDSUFFIX%.dll ECHO Copying "GPUPerfAPICounters%5%BUILDSUFFIX%.dll" into build directory
IF EXIST %1\Bin%BUILDSUFFIX%\%4\GPUPerfAPICounters%5%BUILDSUFFIX%.dll COPY %1\Bin%BUILDSUFFIX%\%4\GPUPerfAPICounters%5%BUILDSUFFIX%.dll %6
IF [%7] == [] GOTO VKSTABLECLOCKS
IF EXIST %1\Bin%BUILDSUFFIX%\%4\GPUPerfAPICounters%5%BUILDSUFFIX%.dll COPY %1\Bin%BUILDSUFFIX%\%4\GPUPerfAPICounters%5%BUILDSUFFIX%.dll %7
IF [%8] == [] GOTO VKSTABLECLOCKS
IF EXIST %1\Bin%BUILDSUFFIX%\%4\GPUPerfAPICounters%5%BUILDSUFFIX%.dll COPY %1\Bin%BUILDSUFFIX%\%4\GPUPerfAPICounters%5%BUILDSUFFIX%.dll %8

:VKSTABLECLOCKS

IF EXIST %2\VkStableClocks%5.exe ECHO Copying "VkStableClocks%5.exe" into build directory
IF EXIST %2\VkStableClocks%5.exe COPY %2\VkStableClocks%5.exe %6
IF [%7] == [] GOTO END
IF EXIST %2\VkStableClocks%5.exe COPY %2\VkStableClocks%5.exe %7
IF [%8] == [] GOTO END
IF EXIST %2\VkStableClocks%5.exe COPY %2\VkStableClocks%5.exe %8

:END

exit 0
