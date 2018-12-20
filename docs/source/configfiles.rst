.. Copyright (c) 2017-2018 Advanced Micro Devices, Inc. All rights reserved.
.. Radeon Compute Profiler Configuration Files

Configuration Files
-------------------

.. _format-of-counter-configuration-file:

Format of counter configuration file (argument passed to ``--counterfile``)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

To specify a set of performance counters to enable when profiling from the
command line, pass the name of a configuration file to the ``--counterfile``
option. The format of this configuration file is one counter name per line.
Counter names are case-sensitive. You can generate a set of counter files
containing every available counter using the following command line:

| ``rcprof --list --outputfile counters.txt``
|

An example of the contents of this file is given below::

  Wavefronts
  VALUInsts
  SALUInsts
  VFetchInsts
  SFetchInsts
  VWriteInsts
  LDSInsts
  GDSInsts
  VALUUtilization
  VALUBusy
  SALUBusy
  FetchSize
  WriteSize
  L1CacheHit
  L1CacheHit
  MemUnitBusy
  MemUnitStalled
  WriteUnitStalled
  LDSBankConflict

.. _format-of-kernellist-configuration-file:

Format of kernel list configuration file (argument passed to ``--kernellistfile``)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

To specify a set of kernels to profile when collecting performance counters
from the command line, pass the name of a configuration file to the
``--kernellistfile`` option. The format of this configuration file is one
kernel name per line. Kernel names are case-sensitive.  When specified, any
kernels dispatched by the application that are not contained in the kernel list
configuration file will not be profiled.

An example of the contents of this file is given below::

  MatrixMultiplyKernel
  binarySearch
  binomial_options

.. _format-of-apirules-configuration-file:

Format of API rules configuration file (argument passed to ``--apirulesfile``)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

To specify a set of rules to use when generating the summary pages from a trace
file when using the command line, pass the name of a configuration file to the
``--apirulesfile`` option. The format of this file is one rule per line in the
``NAME=VALUE`` format. Note that the "VALUE" can be either "True" or "False".

An example of the contents of this file is given below::

  APITrace.APIRules.RefTracker=True
  APITrace.APIRules.BlockingWrite=False
  APITrace.APIRules.BadWorkGroupSize=True
  APITrace.APIRules.RetCodeAnalyzer=True
  APITrace.APIRules.DataTransferAnalyzer=True
  APITrace.APIRules.SyncAnalyzer=True
  APITrace.APIRules.DeprecatedFunctionAnalyzer=True

.. _format-of-apifilter-configuration-file:

Format of API filter configuration file (argument passed to ``--apifilterfile``)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

To ignore a set of APIs when collecting an API trace using the command line,
pass the name of a configuration file to the ``--apifilterfile`` option. The
format of this file is one API name per line.

An example of the contents of this file  for an OpenCL is given below::

  clGetPlatformIDs
  clGetPlatformInfo
  clGetDeviceIDs
  clGetDeviceInfo
  clGetContextInfo
  clGetCommandQueueInfo
  clGetSupportedImageFormats
  clGetMemObjectInfo
  clGetImageInfo
  clGetSamplerInfo
  clGetProgramInfo
  clGetProgramBuildInfo
  clGetKernelInfo
  clGetKernelWorkGroupInfo
  clGetEventInfo
  clGetEventProfilingInfo

.. _format-of-envvar-configuration-file:

Format of environment variable file (argument passed to ``--envvarfile``)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

To specify a set of environment variables to be defined for the application
being profiled, pass the name of a configuration file to the ``--envvarfile``
option. The format of this file is one environment variable per line in the
``NAME=VALUE`` format.

An example of the contents of this file is given below::

  APPLICATION_DATA_DIR=c:\path\to\app\data
  DEBUG_FLAG=True
  LOG_FILE=c:\temp\logfile.log

.. _format-of-occdisplay-configuration-file:

Format of occupancy display configuration file (argument passed to ``--occupancydisplay``)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

A Kernel Occupancy HTML display file can be generated in one of two ways. Both
involve passing a file to the ``--occupancydisplay`` switch.

The first way to generate the HTML file is to pass a previously-generated
.occupancy file to ``--occupancydisplay``. This must be used in conjunction with
the ``--occupancyindex`` switch to specify which occupancy data from the specified
.occupancy file should be used to generate the display file. The argument
passed to ``--occupancyindex`` is a zero-based index.

The second way is a legacy path which involves passing a file manually
generated from an .occupancy file. The format of this configuration file is one
parameter per line in the ``NAME=VALUE`` format. The "VALUES" are taken from a
generated .occupancy file for a particular kernel.

An example of the contents of this file is given below::

  ThreadID=3364
  CallIndex=101
  KernelName=reduce
  DeviceName=Capeverde
  ComputeUnits=10
  MaxWavesPerComputeUnit=40
  MaxWorkGroupPerComputeUnit=16
  MaxVGPRs=256
  MaxSGPRs=512
  MaxLDS=32768
  UsedVGPRs=11
  UsedSGPRs=20
  UsedLDS=4096
  WavefrontSize=64
  WorkGroupSize=256
  WavesPerWorkGroup=4
  MaxWorkGroupSize=256
  MaxWavesPerWorkGroup=4
  GlobalWorkSize=256
  MaxGlobalWorkSize=16777216
  WavesLimitedByVGPR=40
  WavesLimitedBySGPR=40
  WavesLimitedByLDS=32
  WavesLimitedByWorkgroup=40
  Occupancy=80
  DeviceGfxIpVer=6
  SimdsPerCU=4

This second method is currently used by the CodeXL UI. It is much easier to use
the first method when manually generating Occupancy Display files using the
profiler command line.