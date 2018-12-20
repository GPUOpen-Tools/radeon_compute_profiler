.. Copyright (c) 2017-2018 Advanced Micro Devices, Inc. All rights reserved.
.. Radeon Compute Profiler How To

.. _how-to:

How Do I...
-----------

How do I import profiler data into CodeXL for viewing?
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

* CodeXL can be downloaded and installed from
  https://github.com/GPUOpen-Tools/CodeXL/releases
* Once it is installed, launch the CodeXL interface
* Create a new project using the **File->New Project** menu item
* Switch to Profile Mode using the **Profile->Switch to Profile mode** menu
  item
* In the CodeXL Explorer window, right click with the mouse and select the
  **Import Session** menu item.
* Navigate to the location of the .atp or .csv file that you want to import

 * The file will be imported and the data will be displayed

How do I collect a full set of Performance Counters for my application?
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

The full set of available performance counters cannot be collected at the same
time. This is due to a limited number of GPU hardware registers that support
the collection of performance counters.

For most OpenCL applications, the Radeon Compute Profiler is able to reliably
replay individual kernel dispatches so that the full set of performance
counters can be collected for each dispatch. However, for some dispatches, the
profiler is not able to do this (two examples are kernels that take SVM
arguments and kernels that use pipes). This is also the case for all ROCm/HSA
dispatches.

The profiler has support for an alternate method of collecting a full set of
performance counters, which can be used in many cases where individual kernel
replay is not an option. This method is referred to as whole-application replay
and as the name implies, it involves running the entire application more than
once and collecting a different set of performance counters each time.

This method only works well for applications that run deterministically from
one run to the next -- the same kernels must be dispatched in the same order
with the same arguments each time you run the application.

To use this mode, you should specify more than one counter file when executing
the profiler. You do this by including more that one ``--counterfile`` argument
on the profiler command line.  A simple example command line would be:

``rcprof --hsapmc --counterfile counterfile_pass1.txt --counterfile
counterfile_pass2.txt Application``

When running using the above mode, the application will be executed twice in
its entirety. The first time, the counters from ``counterfile_pass1.txt`` will
be collected. The second time, the counters from ``counterfile_pass2.txt`` will
be collected. After both application executions, the profiler will then collate
the results of the two runs into a single output .csv file.

How do I generate counter files to pass to the ``--counterfile`` switch?
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

Counter files containing the full set of support performance counters can be
generated using the following command line:

``rcprof --list --outputfile counterfile.txt``

Several counter files will be generated, one for each supported hardware family
for both OpenCL and ROCm/HSA profiling.

Single-pass counter files (those containing sets of counters that can be
collected in a single pass) can be generated for the hardware available on the
target machine using the following command line:

``rcprof --list --outputfile counterfile.txt --maxpassperfile 1``

How do I determine how many passes are required for a set of counters?
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

Once you have a counter file with the set of counters you wish to collect, you
can determine the number of passes required using the following command line:

``rcprof --counterfile counterfile.txt --numberofpass``

How do I generate a set of single-pass counter files from a given set of counters?
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

Assuming you have a counter file containing a set of counters that require more
than one pass, you can have the profiler split that set of counters into
multiple single-pass counter files for the hardware available on the target
machine using the following command line:

``rcprof --list --counterfile counterfile.txt --maxpassperfile 1 --outputfile singlepasscounters.txt``

How do I force a single-pass profile?
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

You can instruct the profiler to only collect performance counter data that
can be collected in a single pass by adding the ``--singlepass`` switch to the
rcprof command line. This switch causes the profiler to use a counter set that
can be collected in a single pass. For OpenCL profiling, this also prevents the
profiler from collecting timing data for each dispatch, as timing data requires
a separate pass.

Alternatively, you can reduce the number of passes required for OpenCL
profiling by adding the ``--nogputime`` switch to the rcprof command line. This
switch prevents the profiler from collecting timing data for OpenCL kernels.

How do I limit the amount of profiling data generated?
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

When profiling some applications, the default settings used by the profiler can
lead to generation of a lot of data. Sometimes it can be useful to limit the
amount of data generated so you can easily narrow in on particular parts of your
application. The Radeon Compute Profiler has several features which can be used
to limit the amount of profiling data generated.

General features for limiting profiling data
############################################

* ``--startdelay X`` switch

 * Runs the application, but does not start collecting profiling data until the
   specified start delay  (in milliseconds) passes.

* ``--profileduration X`` switch

 * Runs the application, and stops collecting profiling data after the
   specified duration  (in milliseconds) passes.

* ``--startdisabled`` switch

 * Runs the application, but does not collect any profiling data until the
   application calls one of the ActivityLogger entry points to resume
   profiling.

* ActivityLogger instrumentation

 * The ActivityLogger library can be used to instrument an application to
   control which parts of the application generate profiling data.

Features specific to Application Trace mode
###########################################

* ``--apifilterfile filename`` switch

 * Specifies a file which contains a list of APIs to exclude from the output.

* ``--maxapicalls X`` switch

 * Specifies the maximum number of API calls to include in the output (default
   is one million).

Features specific to Performance Counter mode
#############################################

* ``--kernellistfile filename`` switch

 * Specifies a file which contains a list of kernel names to include when
   profiling. Only kernels whose names match names in the file will be profiled.

* ``--maxkernels X`` switch

 * Specifies the maximum number of kernels to profile. Once the limit is
   reached, additional kernel dispatches will not be profiled.

How do I generate HTML Summary Pages?
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

Summary pages can be generated at the same time an Application Trace profile is
performed by adding the ``--tracesummary`` switch to the rcprof command line.

Summary pages can be generated from a previously-generated .atp file using the
following command line:

``rcprof --tracesummary --atpfile filename.atp``

How do I specify which rules should be used when generating Summary pages?
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

By default, all rules are used when generating the BestPractices Summary page.
In order to specify a set of rules, you can pass a configuration file to the
``--apirulesfile`` switch.

Sample command lines are:

``rcprof --tracesummary --apirulesfile rulesfile.txt --apitrace Application``
``rcprof --tracesummary --apirulesfile rulesfile.txt --atpfile filename.atp``

See :ref:`format-of-apirules-configuration-file` for more information on the
expected contents in the rules file.

How do I find out which source code called a particular API call in the Application Trace file (.atp file)?
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

When collecting an application trace, you can ask the profiler to also collect
information about the call location for each API called. To do this, add the
``--sym`` switch to the rcprof command line. With this switch, the profiler
will include a new section in the .atp file that contains the source location
of the call for each API. If the application is built with debug information
this location will be expressed in terms of source file and line number. If no
debug information is available, then the location will be expressed as a code
address.

Note: Using the ``--sym`` switch can add significant overhead when profiling.

If you import an .atp which was generated with the ``--sym`` switch into
CodeXL, the CodeXL UI will allow you to navigate from the API trace to the
source code for the call location.

How do I generate a Kernel Occupancy Display HTML file?
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

See :ref:`format-of-occdisplay-configuration-file` for information on how to
generate a Kernel Occupancy Display file.

As an example, the following command line will create a file called
occupancy2.html for the second item in the specified session.occupancy
input file:

``rcprof --occupancydisplay session.occupancy --occupancyindex 2 -o
occupancy2.html``

How do I annotate the timeline data with application specific events?
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

The Radeon Computer Profiler comes with the AMDTActivityLogger library. This
library provides a simple instrumentation API that allows users to instrument
their code with calls which can be used by Radeon Compute Profiler to annotate
the data in the .atp file generated when collecting an Application Trace. See
the `Activity Logger documentation
<https://github.com/GPUOpen-Tools/common-src-AMDTActivityLogger/blob/master/Doc/AMDTActivityLogger.pdf>`_
for information on using this library.

How do I collect GPU Performance Counters directly from my application without using the profiler?
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

GPUPerfAPI, the library used by Radeon Compute Profiler to collect GPU
performance counters can be used directly by an application to collect
performance counter data at runtime. GPUPerfAPI can be found at
https://github.com/GPUOpen-Tools/GPA.

How do I ensure that my OpenCL application trace contains all API trace data?
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
When the the ``--interval`` command line switch is used (which is the default
and only supported mode on Linux), the application trace might not include the
full trace of all APIs called by the application. This is because any APIs
called after the final interval in the application's lifetime might be omitted.
To limit the number of APIs omitted in this scenario, the Profiler also writes
all queued-up trace data when the clReleaseContext API is called. However, if
an application does not call clReleaseContext to clean up any OpenCL contexts
it has created, or if it calls any OpenCL APIs after the final
clReleaseContext call, then the trace might not contain all APIs called. To
prevent this from happening, it is recommended that clReleaseContext is the
last OpenCL API called by the application when it is running.

How do I report an issue with Radeon Compute Profiler
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

Please use the `GitHub Issues page
<https://github.com/GPUOpen-Tools/RCP/issues>`_ to report any issues or to
request enhancements to the Radeon Compute Profiler.
