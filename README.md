# Radeon Compute Profiler
---

## Overview
The Radeon Compute Profiler (RCP) is a performance analysis tool that gathers
data from the API run-time and GPU for OpenCL™ and ROCm/HSA applications. This
information can be used by developers to discover bottlenecks in the application
and to find ways to optimize the application's performance.

RCP was formerly delivered as part of CodeXL with the executable name
"CodeXLGpuProfiler". Prior to its inclusion in CodeXL, it was known as
"sprofile" and was part of the AMD APP Profiler product.

## Table of Contents
* [Major Features](#major-features)
* [What's New](#whats-new)
* [System Requirements](#system-requirements)
* [Cloning the Repository](#cloning-the-repository)
* [Source Code Directory Layout](#source-code-directory-layout)
* [Documentation](#documentation)
* [Why version 5.x?](#why-version-5x)
* [Known Issues](#known-issues)
* [Building the Source Code](BUILD.md)
* [License](LICENSE)

## Major Features
* Measure the execution time of an OpenCL™ or ROCm/HSA kernel.
* Query the hardware performance counters on an AMD Radeon graphics card.
* Use the CXLActivityLogger API to trace and measure the execution of segments in the program.
* Display the IL/HSAIL and ISA (hardware disassembly) code of OpenCL™ kernels.
* Calculate kernel occupancy information, which estimates the number of in-flight wavefronts on a compute unit as a percentage of the theoretical maximum number of wavefronts that the compute unit can support.
* When used with CodeXL, all profiler data can be visualized in a user-friendly graphical user interface.

## What's New
* Version 5.6 (12/21/18)
  * Adds support for additional GPUs and APUs.
  * Adds support for extracting kernel ISA for OpenCL kernels running on the ROCm stack.
  * Additional Trace summary HTML pages generated for both OpenCL and ROCm/HSA traces.
  * ROCm/HSA: Support for ROCm 2.0.
  * ROCm/HSA: Use new librocprofiler64.so rather than deprecated libhsa-runtime-tools64.so library for performance counter collection and kernel dispatch timing data.
    * Performance counter collection for GFX IPv7 based GPUs is no longer supported on ROCm.
  * ROCm/HSA: There is no longer any difference between using --hsatrace and --hsaaqlpackettrace. Identical data will be produced by both profiling modes.
  * OpenCL: Fixed several issues with profiling on Linux systems with the amdgpu-pro driver installed if both "PAL" and "legacy" OpenCL support was installed by the driver.
  * Fixed an issue where incorrect kernel count displayed in the kernel occupancy output file.
  * Fixed a potential race condition or hang when profiling a plaidML application.
  * Fixed an issue when specifying a relative path to a counter file (--counterfile).
  * 32-bit Linux binaries have been removed, as the OpenCL Linux driver no longer supports 32-bit OpenCL libraries.

## System Requirements
* An AMD Radeon GCN-based GPU or APU
* Radeon Software Adrenaline Edition 18.8.1 or later (Driver Packaging Version 18.30 or later).
* ROCm 2.0. See system requirements for ROCm: https://rocm.github.io/install.html and https://rocm.github.io/hardware.html
  * For use with ROCm, please make sure that the optional ROCm package "rocprofiler-dev" is installed.
    * Ubuntu: sudo apt install rocprofiler-dev
    * CentOS: sudo yum install rocprofiler-dev
* Windows 7, 8.1, and 10
  * For Windows, the `Visual C++ Redistributable for Visual Studio 2015` is required. It can be downloaded from https://www.microsoft.com/en-us/download/details.aspx?id=48145
* Ubuntu (16.04 and later) and CentOS/RHEL (7 and later) distributions

## Cloning the Repository
To clone the RCP repository, execute the following git commands
 * git clone https://github.com/GPUOpen-Tools/RCP.git

After cloning the repository, please run the following python script to retrieve the required dependencies (see [BUILD.md](BUILD.md) for more information):
 * python Scripts/UpdateCommon.py

UpdateCommon.py has replaced the use of git submodules in the CodeXL repository

## Source Code Directory Layout
* [Build](Build) -- contains both Linux and Windows build-related files
* [docs](docs) -- contains documentation sources
* [Scripts](Scripts) -- scripts to use to clone/update dependent repositories
* [Src/CLCommon](Src/CLCommon) -- contains source code shared by the various OpenCL™ agents
* [Src/CLOccupancyAgent](Src/CLOccupancyAgent) -- contains source code for the OpenCL™ agent which collects kernel occupancy information
* [Src/CLProfileAgent](Src/CLProfileAgent) -- contains source code for the OpenCL™ agent which collects hardware performance counters
* [Src/CLTraceAgent](Src/CLTraceAgent) -- contains source code for the OpenCL™ agent which collects application trace information
* [Src/Common](Src/Common) -- contains source code shared by all of RCP
* [Src/DeviceInfo](Src/DeviceInfo) -- builds a lib containing the Common/Src/DeviceInfo code (Linux only)
* [Src/HSAFdnCommon](Src/HSAFdnCommon) -- contains source code shared by the various ROCm agents
* [Src/HSAFdnPMC](Src/HSAFdnPMC) -- contains source code for the ROCm agent which collects hardware performance counters
* [Src/HSAFdnTrace](Src/HSAFdnTrace) -- contains source code for the ROCm agent which collects application trace information
* [Src/HSAUtils](Src/HSAUtils) -- builds a lib containing the Common ROCm code (Linux only)
* [Src/MicroDLL](Src/MicroDLL) -- contains source code for API interception (Windows only)
* [Src/PreloadXInitThreads](Src/PreloadXInitThreads) -- contains source code for a library that call XInitThreads (Linux only)
* [Src/ProfileDataParser](Src/ProfileDataParser) -- contains source code for a library can be used to parse profiler output data files
* [Src/VersionInfo](Src/VersionInfo) -- contains version info resource files
* [Src/sanalyze](Src/sanalyze) -- contains source code used to analyze and summarize profiler data
* [Src/sprofile](Src/sprofile) -- contains source code for the main profiler executable

## Documentation
The documentation for the Radeon Compute Profiler can be found in each [GitHub release](https://github.com/GPUOpen-Tools/RCP/releases). In the release RadeonComputeProfiler-v*.zip file or RadeonComputeProfiler-v*.tgz file, there
will be a "docs" directory. Simply open the index.html file in a web browser to view the documentation.

The documentation is hosted publicly at: http://radeon-compute-profiler-rcp.readthedocs.io/en/latest/

## Why version 5.x?
Although the Radeon Compute Profiler is a newly-branded tool, the technology
contained in it has been around for several years. RCP has its roots in the AMD
APP Profiler product, which progressed from version 1.x to 3.x. Then the profiler
was included in CodeXL, and the codebase was labelled as version 4.x. Now that RCP
is being pulled out of CodeXL and into its own codebase again, we've bumped the
version number up to 5.x.

## Known Issues
* For the OpenCL Profiler
  * When collecting performance counters on Linux, the current user must have write access to <br><br>/sys/class/drm/card\<N\>/device/power_dpm_force_performance_level<br><br>
    where \<N\> is the index of the card in question. By default this file is only modifiable by root, so the profiler would have to be run as root in order for it to modify
    this file. It is possible to modify the permissions for the file instead so that it can be written by unprivileged users. The following command will
    achieve this. Note, however, that changing the permissions on a system file like this could circumvent security. Also, on multi-GPU systems, you may have to replace
    "card0" with the appropriate card number. Permissions on this file may be reset when rebooting the system:
    * sudo chmod ugo+w /sys/class/drm/card0/device/power_dpm_force_performance_level
* For the ROCm Profiler
  * API Trace and Perf Counter data may be truncated or missing if the application being profiled does not call hsa_shut_down
  * Kernel occupancy information will only be written to disk if the application being profiled calls hsa_shut_down
  * When collecting a trace for an application that performs memory transfers using hsa_amd_memory_async_copy, if the application asks for the data transfer timestamps directly, it will not get correct timestamps. The profiler will show the correct timestamps, however.
  * When collecting an aql packet trace, if the application asks for the kernel dispatch timestamps directly, it will not get correct timestamps. The profiler will show the correct timestamps, however.
  * When the rocm-profiler package (.deb or .rpm) is installed along with rocm, it may not be able to generate the default single-pass counter files. If you do not see counter files in /opt/rocm/profiler/counterfiles, you can generate them manually with this command: "sudo /opt/rocm/profiler/bin/CodeXLGpuProfiler --list --outputfile /opt/rocm/profiler/counterfiles/counters --maxpassperfile 1"
  * When collecting a trace for an application with ROCm 2.0, extra HSA API calls not made by the application itself will appear in the trace. These originate from the rocprofiler library which is now used by RCP to collect kernel dispatch timestamps. This issue may go away in future ROCm versions.
