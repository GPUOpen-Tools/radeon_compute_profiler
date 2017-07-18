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
* Version 5.1 (6/28/17)
  * Adds support for additional GPUs, including Vega series GPUs
  * ROCm/HSA: Support for ROCm 1.6
  * Improves display of pointer parameters for some HSA APIs in the ATP file
  * Fixes an issue with parsing an ATP file which has non-ascii characters (affected Summary page generation and display within CodeXL)

## System Requirements
* An AMD Radeon GCN-based GPU or APU
* Radeon Software Crimson ReLive Edition 17.4.3 or later (Driver Packaging Version 17.10 or later).
  * For Vega support, a driver with Driver Packaging Version 17.20 or later is required
* ROCm 1.6. See system requirements for ROCm: https://rocm.github.io/install.html and https://rocm.github.io/hardware.html.
* Windows 7, 8.1, and 10
  * For Windows, the `Visual C++ Redistributable for Visual Studio 2015` is required. It can be downloaded from https://www.microsoft.com/en-us/download/details.aspx?id=48145
* Ubuntu (14.04 and later, 16.04 or later for ROCm support) and RHEL (7 and later) distributions

## Cloning the Repository
To clone the RCP repository, execute the following git commands
 * git clone https://github.com/GPUOpen-Tools/RCP.git

After cloning the repository, please run the following python script to retrieve the required dependencies (see [BUILD.md](BUILD.md) for more information):
 * python Scripts/UpdateCommon.py

UpdateCommon.py has replaced the use of git submodules in the CodeXL repository

## Source Code Directory Layout
* [Build](Build) -- contains both Linux and Windows build-related files
* [Scripts](Scripts) -- scripts to use to clone/update dependent repositories
* [Src/CLCommon](Src/CLCommon) -- contains source code shared by the various OpenCL™ agents
* [Src/CLOccupancyAgent](Src/CLOccupancyAgent) -- contains source code for the OpenCL™ agent which collects kernel occupancy information
* [Src/CLProfileAgent](Src/CLProfileAgent) -- contains source code for the OpenCL™ agent which collects hardware performance counters
* [Src/CLTraceAgent](Src/CLTraceAgent) -- contains source code for the OpenCL™ agent which collects application trace information
* [Src/Common](Src/Common) -- contains source code shared by all of RCP
* [Src/DeviceInfo](Src/DeviceInfo) -- builds a lib containing the Common/Src/DeviceInfo code (Linux only)
* [Src/GPUPerfAPI](Src/GPUPerfAPI) - contains GPUPerfAPI header files that are not yet included in the GPUPerfAPI delivery
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

## Why version 5.x?
Although the Radeon Compute Profiler is a newly-branded tool, the technology
contained in it has been around for several years. RCP has its roots in the AMD
APP Profiler product, which progressed from version 1.x to 3.x. Then the profiler
was included in CodeXL, and the codebase was labelled as version 4.x. Now that RCP
is being pulled out of CodeXL and into its own codebase again, we've bumped the
version number up to 5.x.

## Known Issues
* For the OpenCL™ Profiler
  * Collecting Performance Counters for an OpenCL™ application is not currently working for Vega GPUs on Windows when using a 17.20-based driver. This is due to missing driver support in the 17.20 driver. Future driver versions should provide the support needed.
  * Collecting Performance Counters using --perfcounter for an OpenCL™ application when running OpenCL-on-ROCm is not suported currently. The workaround is to profile using the ROCm profiler (using the --hsapmc command-line switch).
* For the ROCm Profiler
  * API Trace and Perf Counter data may be truncated or missing if the application being profiled does not call hsa_shut_down
  * Kernel occupancy information will only be written to disk if the application being profiled calls hsa_shut_down
  * When collecting a trace for an application that performs memory transfers using hsa_amd_memory_async_copy, if the application asks for the data transfer timestamps directly, it will not get correct timestamps. The profiler will show the correct timestamps, however.
  * When collecting an aql packet trace, if the application asks for the kernel dispatch timestamps directly, it will not get correct timestamps. The profiler will show the correct timestamps, however.
  * When the rocm-profiler package (.deb or .rpm) is installed along with rocm, it may not be able to generate the default single-pass counter files. If you do not see counter files in /opt/rocm/profiler/counterfiles, you can generate them manually with this command: "sudo /opt/rocm/profiler/bin/CodeXLGpuProfiler --list --outputfile /opt/rocm/profiler/counterfiles/counters --maxpassperfile 1"
