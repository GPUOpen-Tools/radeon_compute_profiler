.. Copyright (c) 2017-2018 Advanced Micro Devices, Inc. All rights reserved.
.. Radeon Compute Profiler Commnand Line Documentation

.. _command_line_documentation:

Command Line Documentation
--------------------------

General Usage
~~~~~~~~~~~~~
The main format of the rcprof command line is

``rcprof <options> InputApplication [InputApplication's command line arguments]``

**InputApplication** is the application you want to profile.

Any arguments on the command line after **InputApplication** will be passed as
arguments to the application.

The list of available options is shown in the table below. In general, you
would specify a *Profile Mode*, any desired *General Options*, and any
other arguments that are applicable to that specified profile mode.

There are separate rcprof executables for profiling 64-bit vs. 32-bit
applications. You should use the version of the profiler executable whose
bitness matches the bitness of the application you want to profile.

On Windows, the 64-bit executable is named *rcprof-x64.exe*, while the 32-bit
executable is named *rcprof.exe*.

On Linux, the 64-bit executable is named *rcprof*, while the 32-bit executable
is named *rcprof32*.

If the bitness of the profiler does not match the bitness of the application to
be profiled, a message will be written to the terminal window by rcprof.


Profile Mode options
@@@@@@@@@@@@@@@@@@@@@

.. csv-table::
    :header: "Option", "Description"
    :widths: 45, 55

    "``-t [--apitrace]``", "Trace OpenCL application and generate CPU and GPU
    timestamps and detailed API call traces."
    "``-p [ --perfcounter ]``", "Get the performance counters for each OpenCL
    kernel dispatched by the application."
    "``-A [ --hsatrace]``", "Trace HSA application and generate CPU and GPU
    time stamps and detailed API call traces. (Linux only)"
    "``-C [ --hsapmc]``", "Get the performance counters for each HSA kernel
    kernel dispatched by the application. (Linux only)"
    "``--hsaaqlpackettrace``", "Identical to --hsatrace. Provided for backward
    compatibility (Linux only)"
    "``-O [ --occupancy ]``", "Generate kernel occupancy information file
    (.occupancy). When profiling an HSA application, occupancy information can
    only be collected while also collecting performance counters"
    "``-P [ --occupancydisplay ] arg``", "Path to configuration file to use to
    generate an occupancy display file. Specify the occupancy display file that
    is to be generated with ``--outputfile``. See :ref:`format-of-occdisplay-configuration-file`
    for information about the configuration file format"
    "``-T [ --tracesummary ]``", "Generate summary page from an input .atp file."


General options
@@@@@@@@@@@@@@@

.. csv-table::
    :header: "Option", "Description"
    :widths: 45, 55

    "``--startdisabled``", "Start the application with profiling disabled. This
    is useful for applications that call ``amdtStopProfiling`` and
    ``amdtResumeProfiling`` from the AMDTActivityLogger library."
    "``-d [startdelay] arg``", "Profiling will be enabled after the specified
    delay (in milliseconds). This delay is relative to the first API call
    (either OpenCL or HSA) made by the application."
    "``-D [profileduration] arg``", "Profile duration in milliseconds."
    "``-e [ --envvar ] arg``", "Environment variable that should be defined
    when running the profiled application. Argument should be in the format
    NAME=VALUE."
    "``-E [ --envvarfile ] arg``", "Path to a file containing a list of
    environment variables that should be defined when running the profiled
    application. The file should contain one line for each variable in the
    format NAME=VALUE. See :ref:`format-of-envvar-configuration-file` for
    more information about the configuration file format"
    "``-f [ --fullenv ]``", "The environment variables specified with the
    ``--envvar`` switch represent the full environment block. If not specified,
    then the environment variables represent additions or changes to the system
    environment block."
    "``-l [ --list ]``", "Print a list of valid counter names."
    "``--listactive``", "Print a list of valid counter names for active
    devices."
    "``-L [listdetailed]``", "Print a list of valid counter names with
    descriptions."
    "``--listdetailedactive``", "Print a list of valid counter names with
    descriptions for active devices."
    "``-N [ --sessionname ] arg``", "Name of the generated session. If not
    specified, the name is the parent directory of the OutputFile."
    "``--maxpassperfile arg``", "Limits the set of counters in the generated
    counter file to a set that can be collected in the the specified number of
    passes. If the full set of counters do not fit in the specified number of
    passes, then multiple counter files will be generated. Ignored if
    ``--list`` and ``--outputfile`` are not specified."
    "``--numberofpass``", "Print the number of passes required for the
    specified counter set (or the default set if no counter file is specified)."
    "``-o [ --outputfile ] arg``", "Path to OutputFile. If not provided, the
    default is ``Session1.csv`` in an **RCP** directory  under the current user's
    Documents directory; when performing an API trace, the default is
    ``apitrace.atp`` in the same location.  For Linux, the default location is the
    current user’s home directory. Note: if the specified output file name
    contains the string ``___PID___``, this string will be replaced with a
    string made up of the process id and a unique timestamp in the format
    *pid_timestamp*."
    "``--hsanokerneldemangle``", "Disable the demangling of kernel names (Linux
    only)."
    "``-v [ --version ]``", "Print the rcprof version number."
    "``-w [ --workingdirectory ] arg``", "Set the working directory. If not
    provided, the default is the application binary's path."
    "``-h [ --help ]``", "Print a help message."
    "``--showdoc``", "Launch the documentation in a web browser (if one is available)."


Application Trace mode options (for ``--apitrace`` and ``-–hsatrace`` and ``--hsaaqlpackettrace``)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

.. csv-table::
    :header: "Option", "Description"
    :widths: 45, 55

    "``-F [ --apifilterfile ]``", "Path to the API filter file which contains a
    list of OpenCL or HSA APIs to be filtered out when performing an API
    trace. See :ref:`format-of-apifilter-configuration-file` for information
    about the API filter file format."
    "``-i [ --interval ] arg (=100)``", "Timeout interval in milliseconds.
    Ignored when not performing an API trace and using timeout mode."
    "``-m [ --timeout ]``", "Flush Trace data periodically, default timeout
    interval is 100 milliseconds (can be changed with ``--interval`` switch).
    Ignored when not performing an API trace. (Windows only, this is the
    default mode for Linux.)"
    "``-M [ --maxapicalls ] (=1000000)``", "Maximum number of API calls."
    "``-n [ --nocollapse ]``", "Do not collapse consecutive identical
    ``clGetEventInfo`` calls into a single call in the trace output. Ignored when
    not performing an API trace. "
    "``-r [ --ret ]``", "Always include the OpenCL API return code in API
    trace, even if client application does not query it. Ignored when not
    performing an API trace."
    "``-y [ --sym ]``", "Generate symbol information file (.st) for API trace,
    if available. Ignored when not performing an API trace."


Performance Counter mode options (for ``--perfcounter``  and ``--hsapmc``)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

.. csv-table::
    :header: "Option", "Description"
    :widths: 45, 55

    "``-c [ --counterfile ] arg``", "Path to the counter file to enable
    selected counters (case-sensitive). If not provided, all counters are used.
    Ignored when performing an API trace.  See
    :ref:`format-of-counter-configuration-file` for information about the
    counter file format."
    "``-g [ --singlepass ]``", "Only allow a single pass when collecting
    performance counters. Any counters that cannot fit into a single pass will
    be ignored. If specified, the GPUTime will not be collected, as a separate
    pass is required to query the GPUTime (OpenCL only, this is the default
    for HSA)."
    "``-G [ --nogputime ]``", "Skip collection of GPUTime when profiling a
    kernel (GPUTime requires a separate pass) (OpenCL only, this is the
    default for HSA)."
    "``-k [ --kerneloutput ] arg``", "| Output the specified kernel file (OpenCL only). Valid argument values are:
    |   ``il``:    output kernel IL files
    |   ``isa``:   output kernel ISA files
    |   ``cl``:    output kernel CL files
    |   ``hsail``: output kernel HSAIL files
    |   ``all``:   output all files"
    "``-K [ --kernellistfile ] arg``", "Path to the kernel list file which
    contains a case-sensitive list of kernels to profile. If not provided, all
    kernels will be profiled. See :ref:`format-of-kernellist-configuration-file`
    for information about the kernel list file format."
    "``-s [ --outputseparator ] arg``", "Character used to separate fields in
    the OutputFile. Ignored when performing an API trace."
    "``-x [ --maxkernels] arg (=100000)``", "Maximum number of kernels to
    profile."
    "``--xinitthreads``", "Call XInitThreads at application startup. This can
    be a workaround for an assert that occurs when collecting performance
    counters. (Linux only)"


Trace Summary mode options (for ``--tracesummary``)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

.. csv-table::
    :header: "Option", "Description"
    :widths: 45, 55

    "``-a [ --atpfile ] arg``", "Path to the .atp file from which to generate
    summary pages. Optional when performing an API trace. Required if ``-T`` is
    specified when not performing an API trace. The handle of the kernel."
    "``-R [ --apirulesfile ] arg``", "Path to OpenCL API analyzer configuration
    file. If not specified, all rules are enabled. Ignored when
    ``-–tracesummary`` is not specified.  See
    :ref:`format-of-apirules-configuration-file` for information about the
    configuration file format."


Occupancy display mode options (for ``--occupancydisplay``)
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

.. csv-table::
    :header: "Option", "Description"
    :widths: 45, 55

    "``--occupancyindex``", "Index of kernel to generate an occupancy display
    file for.  This is the index of the kernel within the .occupancy file
    specified as the occupancy configuration file."

Example Command lines
~~~~~~~~~~~~~~~~~~~~~

* Collect default set of OpenCL performance counters:

  ``rcprof --perfcounter "/path/to/app.exe" --device gpu``

* Collect an OpenCL™ API trace:

  ``rcprof --apitrace "/path/to/app.exe" --device gpu``

* Collect kernel occupancy data for all OpenCL kernels dispatched:

  ``rcprof --occupancy "/path/to/app.exe" --device gpu``

* Collect default set of OpenCL performance counters and occupancy data:

  ``rcprof --perfcounter --occupancy "/path/to/app.exe" --device gpu``

* Collect an OpenCL API trace and occupancy data:

  ``rcprof --apitrace --occupancy "/path/to/app.exe" --device gpu``

* Collect an OpenCL API trace with summary pages:

  ``rcprof ---apitrace --tracesummary "/path/to/app.exe" --device gpu``

* Collect default set of HSA performance counters:

  ``rcprof --hsapmc "/path/to/app.exe"``

* Collect an HSA API trace:

  ``rcprof --hsatrace "/path/to/app.exe"``

* Collect default set of HSA performance counters and occupancy data:

  ``rcprof --hsapmc --occupancy "/path/to/app.exe"``

* Collect an HSA API trace with summary pages:

  ``rcprof --hsatrace --tracesummary "/path/to/app.exe"``
  ``rcprof --hsaaqlpackettrace --tracesummary "/path/to/app.exe"``

* Generate summary pages from an .atp file:

  ``rcprof --atpfile "/path/to/output.atp" --tracesummary``

* Generate an occupancy display page for the entry at index 2 within the
  session.occupancy file:

  ``rcprof --occupancydisplay "/path/to/session.occupancy" --occupancyindex 2 -o
  "path/to/output.html"``
