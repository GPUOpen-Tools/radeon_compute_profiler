# Radeon Compute Profiler Release Notes
---

## Version 5.1 (6/28/17)
 * Adds support for additional GPUs, including Vega series GPUs
 * ROCm/HSA: Support for ROCm 1.6
 * Improves display of pointer parameters for some HSA APIs in the ATP file
 * Fixes an issue with parsing an ATP file which has non-ascii characters (affected Summary page generation and display within CodeXL)

## Version 5.0 (in comparison to CodeXL 2.2) (6/2/17)
 * Adds support for additional GPUs and APUs.
 * ROCm/HSA: Support for ROCm 1.5
 * Support for demangling names of HIP and HCC kernels. Requires c++filt to be installed on the system. c++filt can be installed using *sudo apt-get install binutils*
