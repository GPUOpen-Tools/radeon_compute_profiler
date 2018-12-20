.. Copyright (c) 2017-2018 Advanced Micro Devices, Inc. All rights reserved.
.. Radeon Compute Profiler Introduction

Introduction
------------

The Radeon Compute Profiler (RCP) is a performance analysis tool that gathers
data from the API run-time and GPU for OpenCL™ and ROCm/HSA applications. This
information can be used by developers to discover bottlenecks in the
application and to find ways to optimize the application's performance.

Features of the Radeon Compute Profiler include:

* Measure the execution time of an OpenCL or ROCm/HSA kernel.
* Query the hardware performance counters on an AMD Radeon™ graphics card.
* Use the AMDTActivityLogger API to trace and measure the execution of segments
  in the program.
* Store the profile data for each run in a text file.
* Display the IL/HSAIL and ISA (hardware disassembly) code of the kernel for
  OpenCL kernels.
* Show a timeline (including data transfer and kernel dispatch) and an API
  trace for OpenCL or HSA programs (when used in conjunction with the CodeXL
  user interface).
* Calculate and display kernel occupancy info, which estimates the number of
  in-flight wavefronts on a compute unit as a percentage of the theoretical
  maximum number of wavefronts that the compute unit can support.
