#!/bin/bash

set -u

#define path
RCPROOT=`dirname $(readlink -f "$0")`/../..

# Command line args
echo "Command line arguments passed to build_rcp.sh: $*"

# Build Framework (BaseTools/OSWrappers) -- only makes sense to skip if doing an incremental build
bBuildFramework=true
bBuildFrameworkOnly=false

# Build HSA Profiler
bBuildHSAProfiler=true

# Build OCL Profiler
bBuildOCLProfiler=true

# Build Parser Lib
bBuildParserLib=true

# Build Executable
bBuildExecutable=true

# Build XInitthreads
bBuildXInitThreads=true

bBuildDocumentation=false

# Generate zip file
bZip=false

# Only generate zip file
bZipOnly=false

# Only execute clean target (mutually exclusive with incremental build)
bCleanOnly=false

# Build number
BUILD=0

# Incremental build
bIncrementalBuild=false

# Estimate the number of processors we have. This will only work under Linux unfortunately.
CPU_COUNT=${CPU_COUNT:-$(grep -c ^processor /proc/cpuinfo)}

# Append to existing log file
bAppendToLog=false

# Build log file
bLogFileSpecified=false

# Debug build requested
bDebugBuild=false
MAKE_TARGET=
DEBUG_SUFFIX=
CODEXL_FRAMEWORK_BUILD_CONFIG_DIR=release

# HSA directory override
HSA_DIR_OVERRIDE=

# Boost Lib dir directory override
BOOST_LIB_DIR_OVERRIDE=

# Additional compiler defines override
ADDITIONAL_COMPILER_DEFINES_OVERRIDE="ADDITIONAL_COMPILER_DEFINES_FROM_BUILD_SCRIPT="

# Skip HSA build define
SKIP_HSA_BUILD_DEFINE=

SKIP_HSA_MAKE_VAR="SKIP_HSA=0"

# Set build flag
while [ "$*" != "" ]
do
   if [ "$1" = "zip" ]; then
      bZip=true
   elif [ "$1" = "zip-only" ]; then
      bZipOnly=true
   elif [ "$1" = "skip-hsaprofiler" ]; then
      bBuildHSAProfiler=false
      SKIP_HSA_BUILD_DEFINE=-DSKIP_HSA_BUILD
      SKIP_HSA_MAKE_VAR="SKIP_HSA=1"
      ADDITIONAL_COMPILER_DEFINES_OVERRIDE="ADDITIONAL_COMPILER_DEFINES_FROM_BUILD_SCRIPT=$SKIP_HSA_BUILD_DEFINE"
   elif [ "$1" = "skip-oclprofiler" ]; then
      bBuildOCLProfiler=false
   elif [ "$1" = "skip-framework" ]; then
      bBuildFramework=false
      bIncrementalBuild=true
   elif [ "$1" = "skip-parser" ]; then
      bBuildParserLib=false
   elif [ "$1" = "parser-only" ]; then
      bBuildParserLib=true
      bBuildExecutable=false
      bBuildOCLProfiler=false
      bBuildHSAProfiler=false
      SKIP_HSA_BUILD_DEFINE=-DSKIP_HSA_BUILD
      ADDITIONAL_COMPILER_DEFINES_OVERRIDE="ADDITIONAL_COMPILER_DEFINES_FROM_BUILD_SCRIPT=$SKIP_HSA_BUILD_DEFINE"
      bBuildXInitThreads=false
   elif [ "$1" = "framework-only" ]; then
      bBuildFrameworkOnly=true
   elif [ "$1" = "incremental" ]; then
      bIncrementalBuild=true
      bCleanOnly=false
   elif [ "$1" = "quick" ]; then
      bIncrementalBuild=true
      bCleanOnly=false
   elif [ "$1" = "clean" ]; then
      bCleanOnly=true
      bIncrementalBuild=false
   elif [ "$1" = "bldnum" ]; then
      shift
      BUILD="$1"
   elif [ "$1" = "logfile" ]; then
      bLogFileSpecified=true
      shift
      SPECIFIED_LOGFILE="$1"
   elif [ "$1" = "appendlog" ]; then
      bAppendToLog=true
   elif [ "$1" = "debug" ]; then
      bDebugBuild=true
      MAKE_TARGET=Dbg
      DEBUG_SUFFIX=-d
      CODEXL_FRAMEWORK_BUILD_CONFIG_DIR=debug
   elif [ "$1" = "hsadir" ]; then
      shift
      HSA_DIR_OVERRIDE="HSA_DIR=$1"
   elif [ "$1" = "boostlibdir" ]; then
      shift
      BOOST_LIB_DIR_OVERRIDE="BOOST_LIB_DIR=$1"
   elif [ "$1" = "additionaldefines" ]; then
      shift
      ADDITIONAL_COMPILER_DEFINES_OVERRIDE="ADDITIONAL_COMPILER_DEFINES_FROM_BUILD_SCRIPT=$1 $SKIP_HSA_BUILD_DEFINE"
   elif [ "$1" = "doc" ]; then
      bBuildDocumentation=true
   fi
   shift
done

BUILD_ROOT="$RCPROOT/Build"
BUILD_PATH="$BUILD_ROOT/Linux"

echo "RCPROOT=$RCPROOT"

if ($bLogFileSpecified) ; then
   LOGFILE="$SPECIFIED_LOGFILE"
else
   LOGFILE="$BUILD_PATH/RCP_Build.log"
fi

COMMON="$RCPROOT/../Common"
COMMONSRC="$COMMON/Src"
DOCS="$RCPROOT/docs"
BIN="$RCPROOT/bin"
SRCDIR="$RCPROOT/Src"
SRCCOMMON="$SRCDIR/Common"
SRCDEVICEINFO="$SRCDIR/DeviceInfo"
CLCOMMON="$SRCDIR/CLCommon"
SPROFILE="$SRCDIR/sprofile"
RCP_OUTPUT="$RCPROOT/Output"
PROFILER_OUTPUT="$RCP_OUTPUT/bin/"
CLPROFILE="$SRCDIR/CLProfileAgent"
CLTRACE="$SRCDIR/CLTraceAgent"
CLOCCUPANCY="$SRCDIR/CLOccupancyAgent"
HSAFDNCOMMON="$SRCDIR/HSAFdnCommon"
HSAFDNPMC="$SRCDIR/HSAFdnPMC"
HSAUTILS="$SRCDIR/HSAUtils"
HSAFDNTRACE="$SRCDIR/HSAFdnTrace"
PRELOADXINITTHREADS="$SRCDIR/PreloadXInitThreads"
ACTIVITYLOGGER="CXLActivityLogger"
ACTIVITYLOGGERDIR="$COMMONSRC/AMDTActivityLogger/"
GPA="$COMMON/Lib/AMD/GPUPerfAPI/3_3"
VKSTABLECLOCKS="$COMMON/Lib/AMD/VKStableClocks/VKStableClocks/VkStableClocks"
JQPLOT_PATH="$SRCCOMMON/jqPlot"
PROFILEDATAPARSERSRC="$SRCDIR/ProfileDataParser"
PROFILEDATAPARSER="RCPProfileDataParser"

GPACL=libGPUPerfAPICL.so

GPAROCM=libGPUPerfAPIROCm.so

GPACOUNTER=libGPUPerfAPICounters.so

ACTIVITYLOGGERBIN=libCXLActivityLogger.so

GPU_PROFILER_LIB_PREFIX=RCP

RCPROFILEBIN="rcprof$DEBUG_SUFFIX"
CLPROFILEBIN="lib${GPU_PROFILER_LIB_PREFIX}CLProfileAgent$DEBUG_SUFFIX.so"
CLTRACEBIN="lib${GPU_PROFILER_LIB_PREFIX}CLTraceAgent$DEBUG_SUFFIX.so"
CLOCCUPANCYBIN="lib${GPU_PROFILER_LIB_PREFIX}CLOccupancyAgent$DEBUG_SUFFIX.so"
HSAPROFILEAGENTBIN="lib${GPU_PROFILER_LIB_PREFIX}HSAProfileAgent$DEBUG_SUFFIX.so"
HSATRACEAGENTBIN="lib${GPU_PROFILER_LIB_PREFIX}HSATraceAgent$DEBUG_SUFFIX.so"
PRELOADXINITTHREADSBIN="lib${GPU_PROFILER_LIB_PREFIX}PreloadXInitThreads$DEBUG_SUFFIX.so"
PROFILEDATAPARSERBIN="lib${GPU_PROFILER_LIB_PREFIX}ProfileDataParser$DEBUG_SUFFIX.so"

PRODUCTNAME=RadeonComputeProfiler

VERSION_FILE="$SRCCOMMON/Version.h"

VERSION_MAJOR=$(grep -m 1 "RCP_MAJOR_VERSION" $VERSION_FILE | awk '{print $3}')
VERSION_MINOR=$(grep -m 1 "RCP_MINOR_VERSION" $VERSION_FILE | awk '{print $3}')
VERSION="$VERSION_MAJOR.$VERSION_MINOR"
VERSION_STR=v$VERSION.$BUILD

RCP_ARCHIVE_BASE=RadeonComputeProfiler-$VERSION_STR.tgz
RCP_ARCHIVE="$BUILD_PATH/$RCP_ARCHIVE_BASE"

RCPPROFILEDATAPARSERARCHIVE="$PROFILEDATAPARSER-$VERSION_STR.tgz"

if ! ($bZipOnly) ; then
   # delete log file
   if ! ($bAppendToLog) ; then
      rm -f "$LOGFILE"
   fi

   echo "=====Building Radeon Compute Profiler======"  | tee -a "$LOGFILE"

   if ($bBuildFramework) ; then
      #-----------------------------------------
      # build the AMDT framework libraries based on the script of CodeXL
      #-----------------------------------------
      commandLineArgs=$*

      if ($bCleanOnly); then
         commandLineArgs="$commandLineArgs -c"
      fi

      NUM_ERRORS=0

      # Display a start message:
      echo | tee -a "$LOGFILE"
      echo "Building infra projects" | tee -a "$LOGFILE"
      echo "===========================" | tee -a "$LOGFILE"
      echo "Build arguments passed to scons: $commandLineArgs" | tee -a "$LOGFILE"

      if [ -z ${AMD_OUTPUT+x} ]
      then
         # If not, it means this script was invoked by unknown means.
         export AMD_OUTPUT_PROFILING="${RCPROOT}/../"
      fi
      export CXL_common_dir="${COMMON}"
      echo "========================================== " | tee -a "$LOGFILE"
      echo "----------- Start building --------------- " | tee -a "$LOGFILE"
      cd "${RCPROOT}/Build"
      date | tee -a "$LOGFILE"

      if ($bDebugBuild) ; then
         echo "----------- Building debug version --------------- " | tee -a "$LOGFILE"
         echo "scons -C "${RCPROOT}/Build/Linux" CXL_prefix="${RCP_OUTPUT}" CXL_build=debug CXL_build_type=static $commandLineArgs" | tee -a "$LOGFILE"
         (scons -C "${RCPROOT}/Build/Linux" CXL_prefix="${RCP_OUTPUT}" CXL_build=debug CXL_build_type=static CXL_gpu_profiler_backend_dir="$SRCDIR" $commandLineArgs) >> "$LOGFILE" 2>&1
      else
         echo "========================================== " | tee -a $LOGFILE
         echo "scons -C "${RCPROOT}/Build/Linux" CXL_prefix="${RCP_OUTPUT}" CXL_build_type=static $commandLineArgs" | tee -a "$LOGFILE"
         (scons -C "${RCPROOT}/Build/Linux" CXL_prefix="${RCP_OUTPUT}" CXL_build_type=static CXL_gpu_profiler_backend_dir="$SRCDIR" $commandLineArgs) >> "$LOGFILE" 2>&1
      fi
      RC1=$?
      if [ ${RC1} -ne 0 ]
      then
         echo "*** ERROR during the build of the 64 bit framework ***" | tee -a "$LOGFILE"
      fi

      echo "========================================== " | tee -a "$LOGFILE"
      echo "----------- End building ----------------- " | tee -a "$LOGFILE"
      date | tee -a "$LOGFILE"
      echo "========================================== " | tee -a "$LOGFILE"

      NUM_ERRORS=`expr ${NUM_ERRORS} + ${RC1}`
      if [ ${NUM_ERRORS} -ne 0 ]
      then
         echo "*** ERROR ***"
         echo "*** the build failed - see the logs for details ***"
         exit 1
      else
         echo "*** SUCCESS ***"
         if ($bBuildFrameworkOnly) ; then
            exit 0
         fi
      fi

      if ($bCleanOnly); then
         rm -rf "$RCP_OUTPUT"
      fi
   fi

   #-----------------------------------------
   # Update Version.h to include build number
   #-----------------------------------------
   CURRENT_VERSION_H_FILE_PERMISSIONS=$(stat --format %a $VERSION_FILE)
   chmod 777 "$VERSION_FILE"
   old=$(grep -E "#define RCP_BUILD_NUMBER [0-9]+" $VERSION_FILE)
   new="#define RCP_BUILD_NUMBER $BUILD"
   sed -i "s/$old/$new/g" "$VERSION_FILE"
   chmod $CURRENT_VERSION_H_FILE_PERMISSIONS "$VERSION_FILE"

   BUILD_DIRS="$SRCCOMMON $SRCDEVICEINFO $CLCOMMON"

   if $bBuildOCLProfiler ; then
      BUILD_DIRS="$BUILD_DIRS $CLPROFILE $CLTRACE $CLOCCUPANCY"
   fi

   if $bBuildHSAProfiler; then
      BUILD_DIRS="$BUILD_DIRS $HSAFDNCOMMON $HSAUTILS $HSAFDNTRACE $HSAFDNPMC"
   fi

   if $bBuildParserLib; then
      BUILD_DIRS="$BUILD_DIRS $PROFILEDATAPARSERSRC"
   fi

   if $bBuildExecutable; then
      BUILD_DIRS="$BUILD_DIRS $SPROFILE"
   fi

   if $bBuildXInitThreads; then
      BUILD_DIRS="$BUILD_DIRS $PRELOADXINITTHREADS"
   fi


   for SUBDIR in $BUILD_DIRS; do
      BASENAME=`basename $SUBDIR`

      if ! ($bIncrementalBuild) ; then
         make -C "$SUBDIR" spotless >> "$LOGFILE" 2>&1
      fi

      if ! ($bCleanOnly); then
         #make 64 bit
         echo "Build ${BASENAME}, 64-bit..." | tee -a "$LOGFILE"

         if ! make -C "$SUBDIR" -j$CPU_COUNT $SKIP_HSA_MAKE_VAR $HSA_DIR_OVERRIDE $BOOST_LIB_DIR_OVERRIDE "$ADDITIONAL_COMPILER_DEFINES_OVERRIDE" $MAKE_TARGET >> "$LOGFILE" 2>&1; then
            echo "Failed to build ${BASENAME}, 64 bit"
            exit 1
         fi
      fi
   done

   if ! ($bCleanOnly); then
      cp -f "$GPA/Bin/Linx64/$GPACOUNTER" "$PROFILER_OUTPUT"

      if $bBuildOCLProfiler ; then
         cp -f "$GPA/Bin/Linx64/$GPACL" "$PROFILER_OUTPUT"
         cp -f "$VKSTABLECLOCKS" "$PROFILER_OUTPUT"
      fi

      if $bBuildHSAProfiler ; then
         cp -f "$GPA/Bin/Linx64/$GPAROCM" "$PROFILER_OUTPUT"
      fi
   else
      rm -rf "$PROFILER_OUTPUT"
   fi

   if ($bBuildDocumentation); then
      echo "Build Documentation" | tee -a "$LOGFILE"
      if ! make -C "$DOCS" html >> "$LOGFILE" 2>&1; then
         echo "Failed to build Documentation"
         exit 1
      fi
   fi

   #-----------------------------------------
   #clean up bin folder
   #-----------------------------------------
   rm -rf "$BIN"

   if ! ($bCleanOnly); then
      #-----------------------------------------
      #check if bin folder exist
      #-----------------------------------------
      mkdir -p "$BIN"
      mkdir -p "$BIN/$ACTIVITYLOGGER"
      mkdir -p "$BIN/$ACTIVITYLOGGER/bin"
      mkdir -p "$BIN/$ACTIVITYLOGGER/doc"
      mkdir -p "$BIN/$ACTIVITYLOGGER/bin/x86_64"
      mkdir -p "$BIN/$ACTIVITYLOGGER/include"
      mkdir -p "$BIN/jqPlot"

      #-----------------------------------------
      #copy to bin folder
      #-----------------------------------------
      # x64
      if $bBuildExecutable ; then
         cp "$PROFILER_OUTPUT/$RCPROFILEBIN" "$BIN/$RCPROFILEBIN"
      fi

      if $bBuildXInitThreads ; then
         cp "$PROFILER_OUTPUT/$PRELOADXINITTHREADSBIN" "$BIN/$PRELOADXINITTHREADSBIN"
      fi

      if $bBuildOCLProfiler ; then
         cp "$PROFILER_OUTPUT/$CLPROFILEBIN" "$BIN/$CLPROFILEBIN"
         cp "$PROFILER_OUTPUT/$CLTRACEBIN" "$BIN/$CLTRACEBIN"
         cp "$PROFILER_OUTPUT/$CLOCCUPANCYBIN" "$BIN/$CLOCCUPANCYBIN"
         cp "$GPA/Bin/Linx64/$GPACL" "$BIN/$GPACL"
         cp "$VKSTABLECLOCKS" "$BIN/"
      fi
      if $bBuildHSAProfiler ; then
         cp "$PROFILER_OUTPUT/$HSATRACEAGENTBIN" "$BIN/$HSATRACEAGENTBIN"
         cp "$PROFILER_OUTPUT/$HSAPROFILEAGENTBIN" "$BIN/$HSAPROFILEAGENTBIN"
         cp "$GPA/Bin/Linx64/$GPAROCM" "$BIN/$GPAROCM"
      fi
      cp "$GPA/Bin/Linx64/$GPACOUNTER" "$BIN/$GPACOUNTER"

      #AMDTActivityLogger files
      cp "$RCP_OUTPUT/Output_x86_64/$CODEXL_FRAMEWORK_BUILD_CONFIG_DIR/bin/$ACTIVITYLOGGERBIN" "$BIN/$ACTIVITYLOGGER/bin/x86_64/$ACTIVITYLOGGERBIN"
      cp "$ACTIVITYLOGGERDIR/CXLActivityLogger.h" "$BIN/$ACTIVITYLOGGER/include/$ACTIVITYLOGGER.h"
      cp "$ACTIVITYLOGGERDIR/Doc/AMDTActivityLogger.pdf" "$BIN/$ACTIVITYLOGGER/doc/AMDTActivityLogger.pdf"
      #jqPlot files
      cp "$JQPLOT_PATH/"* "$BIN/jqPlot"
   fi
fi

cd "$BUILD_PATH"
rm -f ./*.tgz

#-----------------------------------------
# zip
#-----------------------------------------
if $bZip || $bZipOnly ; then
   cd "$BUILD_PATH"
   rm -f ./*.tgz

   # pack x64 version
   echo "Creating public build tarball..." | tee -a "$LOGFILE"
   mkdir -p "$BUILD_PATH/$PRODUCTNAME-$VERSION/bin"
   if $bBuildDocumentation ; then
      mkdir -p "$BUILD_PATH/$PRODUCTNAME-$VERSION/docs"
   fi

   cp "$BIN/"* "$BUILD_PATH/$PRODUCTNAME-$VERSION/bin"
   cp -R "$BIN/$ACTIVITYLOGGER" "$BUILD_PATH/$PRODUCTNAME-$VERSION"
   cp -R "$BIN/jqPlot" "$BUILD_PATH/$PRODUCTNAME-$VERSION"
   if $bBuildDocumentation ; then
      cp -R "$DOCS/build/html/"* "$BUILD_PATH/$PRODUCTNAME-$VERSION/docs"
   fi
   chmod -R 755 "$BUILD_PATH/$PRODUCTNAME-$VERSION"

   # create artifact for CodeXL
   cd "$BUILD_PATH/$PRODUCTNAME-$VERSION"
   tar cvzf "$RCP_ARCHIVE" bin/ jqPlot/ "$ACTIVITYLOGGER/" docs/
   chmod 755 "$RCP_ARCHIVE"
   cd "$BUILD_PATH"

   if $bBuildParserLib ; then
      # Profile Data parser directory
      mkdir -p "$BUILD_PATH/$PROFILEDATAPARSER-$VERSION/bin"
      mkdir -p "$BUILD_PATH/$PROFILEDATAPARSER-$VERSION/include"

      # copy 64-bit profile data parser binaries
      cp "$PROFILER_OUTPUT/$PROFILEDATAPARSERBIN" "$BUILD_PATH/$PROFILEDATAPARSER-$VERSION/bin/"

      # copy profile data parser header files
      cp "$SRCCOMMON/Defs.h" "$BUILD_PATH/$PROFILEDATAPARSER-$VERSION/include/"
      cp "$SRCCOMMON/IParserListener.h" "$BUILD_PATH/$PROFILEDATAPARSER-$VERSION/include/"
      cp "$SRCCOMMON/IParserProgressMonitor.h" "$BUILD_PATH/$PROFILEDATAPARSER-$VERSION/include/"
      cp "$SRCCOMMON/OSDefs.h" "$BUILD_PATH/$PROFILEDATAPARSER-$VERSION/include/"
      cp "$SRCCOMMON/ProfilerOutputFileDefs.h" "$BUILD_PATH/$PROFILEDATAPARSER-$VERSION/include/"
      cp "$SRCCOMMON/Version.h" "$BUILD_PATH/$PROFILEDATAPARSER-$VERSION/include/"
      cp "$SRCCOMMON/Config.h" "$BUILD_PATH/$PROFILEDATAPARSER-$VERSION/include/"
      cp "$CLCOMMON/CLFunctionEnumDefs.h" "$BUILD_PATH/$PROFILEDATAPARSER-$VERSION/include/"
      cp "$HSAFDNCOMMON/HSAFunctionDefs.h" "$BUILD_PATH/$PROFILEDATAPARSER-$VERSION/include/"
      cp "$PROFILEDATAPARSERSRC/IAtpDataHandler.h" "$BUILD_PATH/$PROFILEDATAPARSER-$VERSION/include/"
      cp "$PROFILEDATAPARSERSRC/IAPIInfoDataHandler.h" "$BUILD_PATH/$PROFILEDATAPARSER-$VERSION/include/"
      cp "$PROFILEDATAPARSERSRC/ICLApiInfoDataHandler.h" "$BUILD_PATH/$PROFILEDATAPARSER-$VERSION/include/"
      cp "$PROFILEDATAPARSERSRC/IHSAApiInfoDataHandler.h" "$BUILD_PATH/$PROFILEDATAPARSER-$VERSION/include/"
      cp "$PROFILEDATAPARSERSRC/IOccupancyFileInfoDataHandler.h" "$BUILD_PATH/$PROFILEDATAPARSER-$VERSION/include/"
      cp "$PROFILEDATAPARSERSRC/IOccupancyInfoDataHandler.h" "$BUILD_PATH/$PROFILEDATAPARSER-$VERSION/include/"
      cp "$PROFILEDATAPARSERSRC/IPerfMarkerInfoDataHandler.h" "$BUILD_PATH/$PROFILEDATAPARSER-$VERSION/include/"
      cp "$PROFILEDATAPARSERSRC/ISymbolFileEntryInfoDataHandler.h" "$BUILD_PATH/$PROFILEDATAPARSER-$VERSION/include/"
      cp "$PROFILEDATAPARSERSRC/ATPParserInterface.h" "$BUILD_PATH/$PROFILEDATAPARSER-$VERSION/include/"
      cp "$PROFILEDATAPARSERSRC/ProfileDataParserLoader.h" "$BUILD_PATH/$PROFILEDATAPARSER-$VERSION/include/"

      # Artifact for profile data parser
      cd $BUILD_PATH/$PROFILEDATAPARSER-$VERSION
      tar cvzf "$BUILD_PATH/$RCPPROFILEDATAPARSERARCHIVE" ./bin ./include
      chmod -R 755 "$BUILD_PATH/$RCPPROFILEDATAPARSERARCHIVE"
      cd $BUILD_PATH
   fi

   # cleanup
   rm -rf "$BUILD_PATH/$PRODUCTNAME-$VERSION/"
   rm -rf "$BUILD_PATH/$PROFILEDATAPARSER-$VERSION"

   # Check artifacts, write to log.
   if [ -e "$RCP_ARCHIVE" ] ; then
      echo "$RCP_ARCHIVE_BASE" >> $LOGFILE
   else
      echo "Failed to generate $RCP_ARCHIVE" >> "$LOGFILE"
      exit 1
   fi

   if $bBuildParserLib ; then
      #Check artifacts of profile data parser
      if [ -e "$BUILD_PATH/$RCPPROFILEDATAPARSERARCHIVE" ] ; then
         echo "$BUILD_PATH/$RCPPROFILEDATAPARSERARCHIVE" >> $LOGFILE
      else
         echo "Failed to generate $BUILD_PATH/$RCPPROFILEDATAPARSERARCHIVE" >> "$LOGFILE"
         exit 1
      fi
   fi
fi

exit 0
