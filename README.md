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
* [Why version 5.x?] (#why-version-5x)
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
* Version 5.0 (in comparison to CodeXL 2.2)
  * Adds support for additional GPUs and APUs.
  * ROCm/HSA: Support for ROCm 1.5
  * Support for demangling names of HIP and HCC kernels. Requires c++filt to be installed on the system. c++filt can be installed using *sudo apt-get install binutils*

## System Requirements
* An AMD Radeon GCN-based GPU or APU
* Radeon Software Crimson Edition 17.2.2 or later (Driver Packaging Version 16.60 or later).
* ROCm 1.5. See system requirements for ROCm: https://rocm.github.io/install.html and https://rocm.github.io/hardware.html.
* Windows 7, 8.1, and 10
** For Windows, the __Visual C++ Redistributable for Visual Studio 2015__ is required. It can be downloaded from https://www.microsoft.com/en-us/download/details.aspx?id=48145
* Ubuntu (14.04 and later) and RHEL (7 and later) distributions

## Cloning the Repository
To clone the RCP repository, execute the following git commands
 * git clone https://github.com/GPUOpen-Tools/RCP.git
After cloning the repository, please run the following python script to retrieve the required dependencies (see [BUILD.md](BUILD.md) for more information):
 * python Scripts/UpdateCommon.py
UpdateCommon has replaced the use of git submodules in the CodeXL repository

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