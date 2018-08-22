# Radeon Compute Profiler Release Notes
---
## Version 5.5 (8/22/18)
 * Adds support for additional GPUs and APUs.
 * ROCm/HSA: Fixes several issues with incorrect or missing data transfer timestamps.

## Version 5.4 (6/22/18)
 * Adds support for additional GPUs and APUs.
 * Support for profiling OpenCL applications running on ROCm
 * OpenCL: Support for tracing OpenCL 2.1 and 2.2 APIs
 * ROCm/HSA: Support for ROCm 1.8.
 * ROCm/HSA: Support for tracing AMD vendor extensions.
 * Fixes an issue parsing occupancy data collected on systems with certain locale settings.
 * ROCm/HSA: Fixes an issue with garbage characters in the .atp file for some HSA API string parameters.
 * OpenCL: Fixes profiling on recent amdgpu-pro drivers using the legacy OpenCL stack.
 * OpenCL: Works around a driver issue where GPU clock frequencies remain fixed after profiling on GFX9-based GPUs.

## Version 5.3 (12/20/17)
 * OpenCL: Support for tracing SSG extension APIs.
 * ROCm/HSA: Support for ROCm 1.7.
 * Documentation now included for RCP (previously RCP documentation was only available in CodeXL).
 * Fixes an issue with incorrect error reporting in the BestPractices Summary Page for OpenCL APIs that do not return a cl_int error code.

## Version 5.2 (9/25/17)
 * Adds support for additional GPUs and APUs.
 * Improves display of HSA agents in the ATP file and in the HSA Trace Summary pages.
 * Improves Best Practices/Warnings/Errors Summary page to track and report mismatched memory allocations and frees.
 * Fixes an issue where incorrect queue index is written to ATP file when profiling an HSA application that dispatches kernels using multiple queues
 * Fixes incorrect transfer sizes shown in CL Context Summary for applications with total transfers over 4GB.
 * Fixes an issue where demangled kernel names with commas caused an invalid .csv file to be generated
 * Adds a speed improvement when profiling HSA applications which dispatch a large number of kernels.

## Version 5.1 (6/28/17)
 * Adds support for additional GPUs, including Vega series GPUs.
 * ROCm/HSA: Support for ROCm 1.6.
 * Improves display of pointer parameters for some HSA APIs in the ATP file.
 * Fixes an issue with parsing an ATP file which has non-ascii characters (affected Summary page generation and display within CodeXL).

## Version 5.0 (in comparison to CodeXL 2.2) (6/2/17)
 * Adds support for additional GPUs and APUs.
 * ROCm/HSA: Support for ROCm 1.5.
 * Support for demangling names of HIP and HCC kernels. Requires c++filt to be installed on the system. c++filt can be installed using *sudo apt-get install binutils*.