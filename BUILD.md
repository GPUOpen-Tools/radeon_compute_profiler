# Radeon Compute Profiler (RCP) Build Instructions
---
## Table of Contents
* [Cloning/Updating Dependent Repositories](#cloningupdating-dependent-repositories)
* [Windows Build Information](#windows-build-information)
* [Linux Build Information](#linux-build-information)
* [Building the Documentation](#building-the-documentation)

## Cloning/Updating Dependent Repositories
RCP no longer uses git submodules to reference dependent repositories. Instead, you need to follow these instructions in
order to clone/update any dependent repositories.

##### Prerequisites
  * This step requires that python be installed on the system. Python can be installed from https://www.python.org/

##### Instructions
 * Simply execute the [UpdateCommon.py](Scripts/UpdateCommon.py) python script located in the [Scripts](Scripts) directory:
   * `python Scripts/UpdateCommon.py`
This script will clone any dependent repositories that are not present on the system. If any of the dependent repositories are already
present on the system, this script will instead do a "git pull" on those repositories to ensure that they are up to date. Please re-run
this script every time you pull new changes from RCP repository.

## Windows Build Information

##### Prerequisites
 * Microsoft Visual Studio 2015 Community Edition or higher + Update 3
 * Windows 10 SDK Version 10.0.10586.0 from https://developer.microsoft.com/en-US/windows/downloads/windows-10-sdk
   * You can override the version of the Windows 10 SDK used by modifying Common/Lib/Ext/Windows-Kits/Global-WindowsSDK.props

##### Build Instructions
 * Load Build\VS2015\RadeonComputeProfiler.sln into Visual Studio
 * Build the 64-bit and/or 32-bit configuration
 * After a successful build, the RCP binaries can be found in __RCP\Output\\$(Configuration)\bin__ (for example RCP\Output\Release\bin)

##### Additional Information
 * The Windows projects each include a .rc file that embeds the VERSIONINFO resource into the final binary. Internally within AMD, a Jenkins build system will dynamically update
   the build number. The version and build numbers can be manually updated by modifying the [Version.h](Src/Common/Version.h) file.

## Linux Build Information

##### Prerequisites
 * RCP uses some shared components which are built using scons. You will need to install scons as follows:
   * `sudo apt-get install scons`
 * RCP requires OpenGL headers in order to build the OpenCL™ profiler backend. You will need to install mesa-common-dev as follows:
   * `sudo apt-get install mesa-common-dev`
 * When building on Ubuntu (16.04 or later), you will need to install the compatible Boost libraries:
   * `sudo apt-get install libboost-all-dev`

##### Building the ROCm/HSA Profiler
* In order to build the ROCm/HSA profiler, you will need all or part of the ROCm stack installed (so that the ROCR header files are available at build time).
* Please see the ROCm install instructions at [https://rocm.github.io/ROCmInstall.html](https://rocm.github.io/ROCmInstall.html), and install either rocm-dkms or rocm-dev, as appropriate.
  * In addition, please make sure that the optional ROCm package "rocprofiler-dev" is installed:
    * Ubuntu: sudo apt install rocprofiler-dev
    * CentOS: sudo yum install rocprofiler-dev
* If the ROCR header files are not available on the build system, you can skip this part of the build. See the Build Instructions section below for information on how to do this.

##### Build Instructions
 * cd into the Build\Linux directory
 * Execute `./build_rcp.sh`
   * By default this performs a from-scratch build of the release version of RCP.
 * The following arguments can be passed to build_rcp.sh to alter the build:
   * `debug`: performs a debug build
   * `skip-framework`: skips building BaseTools/OSWrappers from the CodeXL framework
   * `skip-hsaprofiler`: skips building the ROCm/HSA Profiler agents
   * `skip-oclprofiler`: skips building the OpenCL Profiler agents
   * `quick` or `incremental`: performs an incremental build (as opposed to a from-scratch build)
   * `clean`: performs a "clean" on all build targets, removing all intermediate and final output files
   * `hsadir`: overrides the location of the ROCm/HSA header files (by default they are expected to be in /opt/rocm/hsa)
   * `boostlibdir`: overrides the location of the Boost libraries
   * `doc`: build the documentation using Sphinx (see below for prerequisites)
 * By default, the boostlibdir path is defined to the the following values which are required when building on Ubuntu 16.04:
   * boostlibdir /usr/lib/x86_64-linux-gnu
   * For other systems, you may need to override these paths to point to the correct location of the boost libraries
 * After a successful build, the RCP binaries can be found in __RCP\Output\bin__
 * Example build command line (builds the debug versions of the binaries, skipping the ROCm/HSA profiler):
   * ./build_rcp.sh debug skip-hsaprofiler
 * In addition to using the build_rcp.sh build script to build all of RCP, you can also build a single library by executing __make__ in that library's source code directory. This is useful when making localized changes in a particular library. When using __make__, the following default targets are supported:
   * `\<default\>`: makes the 64-bit release version
   * `Dbg`: makes the 64-bit debug version
 * __Make__ is supported in the following directories (those marked with (*) are required to be built before the others, as they produce static libraries used by the others):
   * Src/CLCommon (*)
   * Src/CLOccupancyAgent
   * Src/CLProfileAgent
   * Src/CLTraceAgent
   * Src/Common (*)
   * Src/DeviceInfo (*)
   * Src/HSAFdnCommon (*)
   * Src/HSAFdnPMC
   * Src/HSAFdnTrace
   * Src/HSAUtils (*)
   * Src/PreloadXInitThreads
   * Src/ProfileDataParser
   * Src/sprofile
 * When using __make__ to build the ROCm/HSA agents, by default the HSA headers are expected to be in /opt/rocm/hsa. You can override this by specifying "HSA_DIR=<dir>" on the make command line:
   * Example: `make Dbg HSA_DIR=/home/user/hsa_dir`

## Building the Documentation

##### Prerequisites
 * A working python installation is required
 * Install Sphinx:
   * `pip install Sphinx`
 * Install Sphinx Read The Docs theme:
   * `pip install sphinx-rtd-theme` (read the docs theme is not installed by default)
 * Install spelling checker (optional)
  * `pip install pyenchant`
  * `pip install sphinxcontrib-spelling`

##### Building
 * Once you have Sphinx installed, simply run the following command from the "docs" subdirectory:
   * `make html`
 * To run the spelling checker:
   * edit conf.py and uncomment the line adding in the `sphinxcontrib.spelling` extension
   * `make spelling`
