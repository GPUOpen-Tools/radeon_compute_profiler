# Radeon Compute Profiler Release Notes
---
## Version 5.2 (9/25/17)
 * Adds support for additional GPUs and APUs.
 * Improves display of HSA agents in the ATP file and in the HSA Trace Summary pages.
 * Improves Best Practices/Warnings/Errors Summary page to track and report mismatched memory allocations and frees.
 * Fixes an issue where incorrect queue index is written to ATP file when profiling an HSA application that dispatches kernels using multiple queues
 * Fixes incorrect transfer sizes shown in CL Context Summary for applications with total transfers over 4GB.
 * Fixes an issue where demangled kernel names with commas caused an invalid .csv file to be generated
 * Adds a speed improvement when profiling HSA applications which dispatch a large number of kernels.

## Version 5.1 (6/28/17)
 * Adds support for additional GPUs, including Vega series GPUs
 * ROCm/HSA: Support for ROCm 1.6.
 * Improves display of pointer parameters for some HSA APIs in the ATP file.
 * Fixes an issue with parsing an ATP file which has non-ascii characters (affected Summary page generation and display within CodeXL).

## Version 5.0 (in comparison to CodeXL 2.2) (6/2/17)
 * Adds support for additional GPUs and APUs.
 * ROCm/HSA: Support for ROCm 1.5.
 * Support for demangling names of HIP and HCC kernels. Requires c++filt to be installed on the system. c++filt can be installed using *sudo apt-get install binutils*.
