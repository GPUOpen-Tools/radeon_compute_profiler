.. Copyright (c) 2017-2018 Advanced Micro Devices, Inc. All rights reserved.
.. Radeon Compute Profiler Configuration Files

Output Files
------------

Performance Counter Output (SESSION_NAME.csv)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

This comma-delimited file is generated when a profile collects performance
counters.

The file starts with a file header section (in comments) that indicates the
Profiler version number and information about the application that was
profiled. Following the file header is a line containing the list of the
column headers. The first several fields contain statistics about the dispatch.
The rest of the fields in this row represent the performance counters that were
collected.

Each additional line contains data collected by the Profiler. There will be one
line for each kernel dispatched by the profiled application.

The following dispatch statistics are collected for both OpenCL and HSA
dispatches:

.. csv-table::
    :header: "Name", "Description"
    :widths: 45, 55

    "Method", "The kernel name (appended with the Device Name)."
    "ExecutionOrder", "The order of execution for the kernel dispatch
    operations in the program."
    "ThreadID", "The thread ID of the host thread that made the HSA API call
    that initiated the kernel dispatch operation."
    "GlobalWorkSize", "The global work-item size of the kernel."
    "WorkGroupSize", "The work-group size of the kernel."
    "LocalMemSize", "The amount of local memory (LDS) in bytes being used by
    the kernel."
    "VGPRs", "The number of general-purpose vector registers used by the
    kernel."
    "SGPRs", "The number of general-purpose scalar registers used by the
    kernel."
    "KernelOccupancy", "The kernel occupancy. This is an estimate of the number
    of in-flight wavefronts on a compute unit as a percentage of the
    theoretical maximum number of wavefronts that the compute unit can
    support."

In addition, the following dispatch statistics are collected for OpenCL
dispatches:

.. csv-table::
    :header: "Name", "Description"
    :widths: 45, 55

    "CallIndex", "The call index of the OpenCL API call that initiated the
    kernel dispatch operation."
    "Time", "The time spent (in milliseconds) executing the kernel. This does
    not include the kernel set-up time."
    "ScratchRegs", "The number of scratch registers used by the kernel (valid
    only for GPU devices). If non zero, this typically is the main bottleneck.
    To reduce this number, reduce the number of GPRs used by the kernel."

For the list of supported performance counters and their descriptions, see
:ref:`supported-perf-counters`

Application Trace Output (SESSION_NAME.atp)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

This file is generated when performing a profile that collects an application
timeline trace. The file starts with a file header section which contains the
trace file version number, the Profiler version number, and information about
the application that was profiled. Following the file header are several
sections: the first section contains the API Trace data for the profile
session; the second contains timestamp data for the profile session. For HSA
traces that include HSA kernel dispatches, there will be a section containing
the kernel dispatch timestamp data.  If the ``--sym`` option is specified,
there will be a section containing the source code information for the profile
session. If the application is instrumented using the ActivityLogger library,
there will be a PerfMarker section containing the Begin/End Marker
entries and timestamps.

API Trace Section
#################

The API Trace section is marked with one of the following lines:

``=====ocl API Trace Output=====``

``=====hsa API Trace Output=====``

The API Trace section contains one or more thread blocks.

An API Trace thread block consists of the following:

* A line giving the thread ID.
* A line giving the number of APIs for that thread, followed by a line for each
  API.

Each API is listed in the format: ReturnValue = APIName ( ParameterList ).

The ParameterList is a semi-colon delimited list of the parameters passed to
the API.

Timestamp Section
#################

The Timestamp section is marked with one of the following lines:

``=====ocl Timestamp Output=====``

``=====hsa Timestamp Output=====``

The Timestamp section contains one or more thread blocks. In the Timestamp
section, all time counter data represents CPU-based time expressed in
nanoseconds. A Timestamp thread block consists of the following:

* A line giving the thread ID.
* A line giving the number of APIs for that thread, followed by an API line for
  each API. An API line consists of at least 4 pieces of data:

 * An integer representing the API type.
 * A string showing the API name.
 * The time counter value for the start of the API.
 * The time counter value for the end of the API.

Most OpenCL Enqueue APIs contain the following additional data, appended to
the end of the API line.

* An integer representing the enqueue command type.
* A string showing the enqueue command name.
* The time counter value for the time the command was queued by the host – this
  corresponds to CL_PROFILING_COMMAND_QUEUED.
* The time counter value for the time the command was submitted by the host to
  the target device – this corresponds to CL_PROFILING_COMMAND_SUBMIT.
* The time counter value for the time the command started executing on the
  target device – this corresponds to CL_PROFILING_COMMAND_START.
* The time counter value for the time the command finished executing on the
  target device – this corresponds to CL_PROFILING_COMMAND_END.
* The unique numerical ID of the queue.
* The handle of the queue.
* The unique numerical ID of the context.
* The handle of the context.
* The device name.

OpenCL Kernel dispatch Enqueue commands contain the following additional data
appended to the end of the API line:

* The handle of the kernel.
* The name of the kernel.
* The global work size for the kernel – one value is given for each work
  dimension.
* The work-group size for the kernel – one value is given for each work
  dimension.

OpenCL Data transfer Enqueue commands contain the data transfer size appended
to the end of the API line.

HSA Data transfers (entries for ``hsa_amd_memory_async_copy``) will also
contain the time counter value for the start and completion time for the
data transfer.

HSA Kernel Timestamp Section
############################

The HSA Kernel Timestamp section is marked with the following line:

``=====hsa Kernel Timestamp Output=====``

The HSA Kernel Timestamp section contains the following information:

* A line giving the number of HSA kernel dispatches, followed by a Kernel
  Timestamp line for each kernel dispatched by the application. A Kernel
  Timestamp line consists of the following pieces of data:

 * A string showing the kernel symbol name.
 * The handle of the kernel.
 * The time counter value for the time the kernel started executing on the
   device.
 * The time counter value for the time the kernel finished executing on the
   device.
 * The name of the agent the where the kernel was dispatched.
 * The handle of the agent where the kernel was dispatched.
 * The zero-based index of the queue that was used to dispatch the kernel.
 * The zero-based agent index for the agent where the kernel was dispatched.
 * The AQL Packet type.
 * The AQL Packet ID.
 * The string representation of the AQL dispatch packet structure.

Source Code Section
###################

The Source Code section contains one or more thread blocks. A Source Code
thread block consists of the following:

* A line giving the thread ID.
* A line giving the number of APIs for that thread, followed by a Source Code
  line for each API. A Source Code line consists of the following 4 pieces of
  data:

 * A string showing the API name.
 * A string showing the name of the function that called the API (or an address
   if no debug information was found).
 * An integer representing the line number for the location of the API call.
 * A string showing the name of the file for the location of the API call (this
   is not shown if no debug information was found).

Perf Marker Section
###################

The Perf Marker section is marked with the following line:

``=====Perfmarker Output=====``

The Perf Marker section contains one or more thread blocks. In the Timestamp
section, all time counter data represents CPU-based time expressed in
nanoseconds. A Timestamp thread block consists of the following:

* A line giving the thread ID.
* A line giving the number of entries for that thread, followed by a PerfMarker
  line for each entry. A PerfMarker line consists of either a BeginPerfMarker
  entry or an EndPerfMarker entry.
* A BeginPerfMarker entry is made up of the following pieces of data:

 * The string ``clBeginPerfMarker``
 * The name of the Perf Marker
 * The time counter value for the time the application called BeginMarker
 * The group name of the Perf Marker

* An EndPerfMarker entry is made up of the following pieces of data:

 * The string ``clEndPerfMarker``
 * The time counter value for the time the application called EndMarker

Markers can be nested, but the each EndMarker entry serves as the end time
value for the most recently-called BeginMarker entry.

Kernel Occupancy Output (SESSION_NAME.occupancy)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

This comma-delimited file is generated when a profile collects kernel occupancy
information.

The file starts with a file header section (in comments) that indicates the
Profiler version number and information about the application that was
profiled. Following the file header is a line containing the list of names of
the data used in order to compute kernel occupancy.

Each additional line contains data collected by the Profiler. There will be one
line for each kernel dispatched by the profiled application to a GPU device.
