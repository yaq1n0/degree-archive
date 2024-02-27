
Real-time Embedded Systems : Lab 1
===================================

Due date: Wednesday 16th of Nov at 22:00

This lab is intended to provide practical experience of some of the
following aspects from the start of the course:

- Storing real-time entity state in a database
- Improving and optimising code to reduce $d_{computer}$.
- Running multiple tasks interacting through a real-time database.
- Measuring and characterising jitter with multiple tasks.

This is an individual lab. You are welcome to talk to other students
about what you are doing, but do not share code or results.

To reduce assessment burden the lab work has been simplified
and abstracted so that it can run in a standard laptop or
desktop without platform dependencies. This somewhat disconnects
it from reality, but is necessary in order to avoid substantial
overhead due to installing libraries or forcing exactly one
platform.

Version history
===============

- 2022/11/04 : Relaxing the choice of x-axis variable for 1.3 and 1.5.
    - Thanks to Charlie Britton and dp1u20.
- 2022/10/31 : Updated makefile to add explicit dependency on headers
    - Thanks to Fred Harrison and George Orchard for identifying this.
- 2022/10/31 : Emphasised the correct (later) due date in the readme
    - Thanks to Stanislav Modrak for pointing out inconsistencies with dates.
    - The reason for moving from 14th to 16th is that 3rd year hand-in is supposed to be on a Wednesday according to the "hand-in" guidelines.
- 2022/10/27 : Tweaked to make it build clean with clang on OSX.
    - Thanks to George Orchard for helping to fix this.
- 2022/10/26 : Fix to parameter ordering error in readme.
    - Thanks to Charlie Britton for noticing it and supplying pull request.
- 2022/10/24 : First public version

Setup
=========

Building
--------

The source code is structured for a posix-like environment, but is expected
to be portable to any C++11 compatible environment, including Linux,
MacOS, and Windows. The explicit C++11 flag is included for compatibility
with building on Apple, but you are free to use C++14 or later if you wish.

### Linux/Posix

The included makefile should be able to build executables into the
directory `bin`, for example:
```
$ make bin/yes
```
will compile `src/yes.cpp` into the program `bin/yes`.

As a convenience you can use the following targets:

-  `make all_distrib` : Builds everything included with the distribution
-  `make all_deliverable` : Checks that everything expected for the submission either exists (pdfs) or can be built (programs).

### Windows

I would recommend installing [WSL](https://learn.microsoft.com/en-us/windows/wsl/install), then just compiling and running things under Linux.
Or if you preferr, booting an Ubuntu virtual machine, then doing it there.

However, all code should run in Microsoft compilers, so if you prefer Visual Studio
please feel free to create workspaces/projects and so on. These are not relevant for the
submission, but if project work-space files are included that is not a problem. Just
watch out for the size of the eventual zip file as it should be <10MB.

### Macs

I don't have access to a Mac, and Apple choose to make running the OS in a virtual machine
extremely difficult (both in terms of technology and legally), so it is very difficult to
test any of this. That said, it should work on either x86 or ARM-based Macs, as long as
you have a c++11 or later compiler. Beware the default compiler on OSX, as it is often very old.
Previously I have had reasonable experiences using [`brew`](https://brew.sh/) or [`ports`](https://www.macports.org/)
on OSX to get a functioning development environment.

Update (2022/10/27): as long as `-std=c++11` is specified explicitly it seems to work on OSX clang. Note
the code has been tweaked to make it pure C++11 in response, but you can use later C++ standards if you wish.

Compilation
-----------

Your code and code changes should be limited to just the source files
mentioned in the deliverables. Regardless of what build system you
choose to use, during assessment the programs will be compiled purely
by compiling the program .cpp file with the `include` directory in
the search path.

Do not split the files up into extra headers, and particularly don't
add code into extra source files as I won't know what they are called
when I build the programs.

You should respect the two interfaces being used (`RealTimeDatabase`
and `Task`), and not make assumptions about users or implementors.
If you change something on those interfaces (e.g. adding something to
the virtual base class) you are guaranteeing that your code won't work
with other implementations.

It is a minimum expectation that code compiles: it is trivial to test
whether code compiles, and it is better to submit something that is
commented out rather than something that doesn't compile. If graphs
appear but the submitted code doesn't compile then the graphs will
be treated with some suspicion. In such cases a demo may be requested,
to demonstrate how the submitted code could be used to create the
submitted graphs.

It is a fact of life that sometimes there are mild differences between
C++ environments, and sometimes things will compile on one platform
but not compile on others due to different default header compilation.
You can assume that the assessor is intimately aware of what might
change between platforms, and will be able to tell the difference
between code that happens to compile correctly on one platform versus
failing on another (usually due to headers), and code that would never
compile on any platform.



Real-time Database
------------------

The class `RealTimeDatabase` in file `include/real_time_database.hpp` defines
the interface for a key-value database. The class contains five pure virtual
methods which should be implemented, plus a number of non-virtual helper
functions. The class file contains documentation for the overall class and
the individual methods.

There is one given implementation of
`RealTimeDatabase`, and three expected implementations to be created.

Name        | Class name                  | Given?      | Thread-safe? | Description
------------|-----------------------------|-------------|--------------|--------------------------------------------
`naive`     | `NaiveRealTimeDatabase`     | Given       | No           | Very simple single-threaded implementation
`locked`    | `LockedRealTimeDatabase`    | Deliverable | Yes          | Multi-threaded implementation. Performance is irrelevant.
`efficient` | `EfficientRealTimeDatabase` | Deliverable | Optional     | Fast single-threaded implementation.
`scalable`  | `ScalableRealTimeDatabase`  | Deliverabe  | Yes          | Fast multi-threaded implementation.

Graphs
------

When asked for a graph of results, you should prepare a single page pdf with
the given file-name containing a line graph. Standard expectations for a graph in a project report
or other publication apply. These include:

- x-axis has a label.
- x-axis represents parameter being varied or controlled.
- y-axis has a label.
- y-axis represents thing being measured or observed.
- Graph has a title describing what it is.
- Any fixed parameter values are identified (either in a caption or in title)
- If there are multiple lines there is a legend
- Measured data-points are visible.
- Measured data-points are connected by lines.
- An appropriate number of data-points
- Graph is in a vector format (i.e. if you zoom in you can't see pixels)
- Asymptotic scaling is demonstrated by showing asymptotic curve alongside empirical data.

In this particular coursework you should also include the following information on the graph:

- Operating system used
- Processor begin used

Q1 : Database implementations
=============================

The program `src/benchmark_real_time_database.cpp` will instantiate a chose real-time database
implementation, and apply a work-load with the following parameters:

- Implementation : default=naive
- NumAccesses : default=1e7
- NumKeys : default=100
- NumThreads : default=1

When you run `benchmark_real_time_database` program you can pass the four parameters
as command-line arguments, for example:
```
$ bin/benchmark_real_time_database naive 1000 10 1
```
will run the naive engine with 1000 accesses, 10 keys, and 1 thread.

The `benchmark_real_time_database` program measures the following values for a given
set of parameters:

- CheckSumFailures : Number of errors found while reading values previously written
- HardwareConcurrency : Number of hardware threads in the system (as reported by `std::thread::hardware_concurrency`)
- WallTime : Total wall-clock time taken to execute all accesses

When run the program will print out some progress information to `stderr`, then print results
as [CSV](https://en.wikipedia.org/wiki/Comma-separated_values) to `stdout`. For example:
```
$ bin/benchmark_real_time_database naive 100000 100 1
Name=naive, NumAccess=100000,NumEntries=100,NumThreads=1
Found 0 failures in 100000 updates across 1 threads.
Database,Accesses,Entries,Threads,CheckSumFailures,HWThreads,WallTime
naive,100000,100,1,0,4,1.36409
```
The first two lines are printed to `stderr`, while the last two are printed to `stdout`.
If you want to capture just the output, you can redirect to a file:
```
$ bin/benchmark_real_time_database naive 100000 100 1 > output.csv
Name=naive, NumAccess=100000,NumEntries=100,NumThreads=1
Found 0 failures in 100000 updates across 1 threads.
```
which will result in a file called `output.csv` containing the results.

Currently the `naive` implementation has WallTime = O(NumAccesses * NumKeys). So if you double
the number of updates execution time doubles (as expected), and if you double the number
keys the execution time also doubles (which is bad).

Complete the other implementations to achieve the following:

**Task 1.1** : `locked` : CheckSumFailures=0 with NumThreads>=1

**Task 1.2** : `efficient` : WallTime is O(NumAccesses) and CheckSumFailures=0 with NumThreads=1

**Task 1.3** : produce an experimental graph called `graphs/1.3-efficient_db_access_scaling.pdf` that plots
NumAccesses versus WallTime, or NumKeys versus WallTime,  to demonstrate objective 1.2 has been met.

_Note (2022/11/04): Originally this said NumAccesses versus WallTime, which implicitly requires multiple lines for different values of NumKeys._
_People pointed out that NumKeys might be more natural to prove the point, which is fair enough. But however it is done,_
_you need more than one line - you can't really show it has gone from O(NumAccess*NumKeys) to O(NumAccesses) without varying both NumAccesses and NumKeys._ 
_For assessment purposes assume that there will be a wide input filter as long as the experiments supports that it is now efficient._

**Task 1.4** : `scalable` : WallTime is O(NumAccesses) and CheckSumFailures=0 with NumKeys >> NumThreads and 1<=NumThreads<=HardwareConcurrency.

**Task 1.5** : produce an experimental graph called `graphs/1.5-scalable_db_update_scaling.pdf` that plots
NumAccesses versus WallTime, or NumKeys versus WallTime, to demonstrate objective 1.4 has been met.

_Note (2022/11/04): Updated to NumAccesses or NumKeys for same reason as above. Either approach will be_
_acceptable for assessment._

_Note: scaling with number of threads is often empirically messy. Focus on making_
_the access time from multiple threads efficient, and don't worry if you don't_
_see perfect scaling with threads in practise. However, if WallTime is not lower_
_when going from 1 thread to 2 then something is not right._

**Task 1.6** : `scalable`: WallTime is O(1/NumThreads) and CheckSumFailures=0
    _with_ NumUpdates=1e8 _and_ NumKeys >> NumThreads _and_ 1<=NumThreads<=HardwareConcurrency.

**Task 1.7** : produce an experimental graph called `graphs/1.7-scalable_db_thread_scaling.pdf` that plots
NumThreads versus WallTime to demonstrate objective 1.6 has been met.

Q2 : Task scheduling
===============================

The file `include/task.hpp` defines a virtual base-class for "tasks", which are
things we might want to schedule. Each task exposes a function called `Execute`,
which does whatever that task does, and a function `GetNextDeadline` which
gives the absolute time at which it wants execute to be called next.

There are three tasks defined in the directory `include/tasks`:

- `ComputeTask` : This task reads a set-point "volume", and uses that to
    compute a value "result". The time taken to compute "result" increases
    as "volume" increases, and can take different amounts of time.
    100ms after calculating "result" it starts again.

- `InputTask` : This task reads characters from the keyboard. If it receives
  a `+` then "volume" increases 10%, on a `-` "volume" decreases 10%.

- `OutputTask` : This task executes every 10ms, and every second it also
   prints its own scheduling jitter plus the current values of "volume" and "result".
   The values printed out appear as a single CSV line, and all track the
   scheduling "error", which is the time difference between when the task
   was supposed to be scheduled and when it actually started.

_These three tasks are completely contrived, and don't do anything useful._
_However, they do represent problems that occur in real embedded systems,_
_where tasks with very different characteristics need to co-operate_
_in a single system. There are a couple of demos I'll show later to_
_give real-world examples, but for now we are staying abstract to_
_reduce cognitive load._

The three programs `src/mini_system_queue_*task.cpp` include different
variants of the system, with a simple queueing scheduler that waits
until the earliest deadline amongst all tasks:

- `src/mini_system_queue_1task.cpp` : Has just `OutputTask`.
- `src/mini_system_queue_2task.cpp` : Includes both `OutputTask` and `ComputeTask`, with "volume"=1000000. 
- `src/mini_system_queue_3task.cpp` : Includes `OutputTask`, `ComputeTask`, and `InputTask`.

**Task 2.1** : Run `mini_system_queue_1task` and produce a graph `graphs/2.1-queue_task1_quiet.pdf`
    which shows the batch maximum error, and batch standard deviation over a 10 second period.

**Task 2.2** : Run `mini_system_queue_1task` again, but this time use any
    technique you can think of to increase jitter about half-way through
    the 10 second run. Plot the results in a graph called `graphs/2.2-queue_task1_high_jitter.pdf`
    showing the batch maximum error and batch standard deviaiton.
    Add an indicator on the graph to show where the intervention started and include a
    brief note on the graph about what the intervention was.

Run `mini_system_queue_2task` and explore the impact of having the
variable compute load running along-side the time-critical paths.
You are not required to produce an specific graphs, but try to
explain to yourself what is happening.

The program `mini_system_queue_3task` contains `InputTask` which does a
blocking read from `stdin` every 100ms. If you try to run it, you'll
find it freezes unless you type things into the keyboard. It is also
line-buffered, so you'll find that you need to type return in order for
characters to make it through to the program. 

We can fake a human typing by using the `yes` program included in most
Posix-like environments, or you can use the program `src/yes.cpp` if needed.
The command `yes +` will produce an infinite number of `+` characters, so
we can sent the input into our program as follows:
```
$ yes + | bin/mini_system_queue_3task
```

**Task 2.3** : Run `bin/mini_system_queue_3task` and plot a graph `graphs/2.3-queue_volume_versus_latency.pdf`
    showing the value of "volume"
    against the batch mean error and batch worst-case error for a stream of '+' characters.
    Include an annotation on the graph which highlights a phase change, and
    describe in one sentence what is happening.

**Task 2.4** : Create a new program `src/mini_system_threads_3task.cpp` which runs each
task in a separate thread.

_Hint_: the while loop in `src/mini_system_queue_1task.cpp` manages execution of
a single task in a single thread. You just need to create three threads, one for
each of the tasks.

_Hint_: don't forgot to make sure you instantiate a thread-safe database instance
such as `locked` or `scalable`.

_Hint_: if you have done this correctly `mini_system_queue_1task` should print
the jitter results even if you don't press any keys to providing input.

**Task 2.5** : Run `bin/mini_system_threads_3task and plot a graph
    `graphs/2.5-threads_volume_versus_latency.pdf` the value of "volume"
    against the batch mean error and worst-case error for a stream of '+' keys.


Deliverables
============

The set of assessed deliverables are:

| Task | File to be modified                               | Weight |  Activity                               |
| -----|---------------------------------------------------|--------|-----------------------------------------|
| 1.1  | `include/databases/LockedRealTimeDatabase.hpp`    |   2    | Make db thread-safe                     |
| 1.2  | `include/databases/EfficientRealTimeDatabase.hpp` |   2    | Speed independendent of keys            |
| 1.3  | `graphs/1.3-efficient_db_access_scaling.pdf`      |   2    | Demonstrate scaling of `efficient`      |
| 1.4  | `include/databases/ScalableRealTimeDatabase.hpp`  |   2    | Thread-safe and efficient               |
| 1.5  | `graphs/1.5-scalable_db_update_scaling.pdf`       |   2    | Demonstrate `scalable` database         |
| 1.6  | `include/databases/ScalableRealTimeDatabase.hpp`  |   4    | Wall time decreases with more threads   |
| 1.7  | `graphs/1.6-scalable_db_thread_scaling.pdf`       |   3    | Demonstrate time reduction with threads |
| 2.1  | `graphs/2.1-queue_task1_quiet.pdf`                |   2    | Characterise jitter in quiet system     |
| 2.2  | `graphs/2.2-queue_task1_high_jitter.pdf`          |   2    | Characterise jitter with interventions  |
| 2.3  | `graphs/2.3-queue_volume_versus_latency.pdf`      |   2    | Identify phase transition with "volume" |
| 2.4  | `src/mini_system_threads_3task.cpp`               |   4    | Move scheduling to multiple threads     |
| 2.5  | `graphs/2.5-queue_volume_versus_latency.pdf`      |   3    | Demonstrate low jitter with threads     |

The weight gives the relative weight of each task, with a total mark out of 30.

Your submission should be a zip file that contains these files in the
same relative directories.

These files are the only ones _required_ to be submitted, but it is a good idea to submit
the whole directory, particularly if you are using git or other source control
to track your work. The only caveat is that your submitted zip file should
be less than 10MB, so you might need to clean out build artefacts.

Assessment:

- Assessment will be via a combination of functional tests and human inspection.
- You can assume that your code will be compiled and run.
- There are no marks for coding style of commenting. The only thing that matters is if it works.
- Platform compilation problems will be fixed for free.
- Minor compilation or run-time problems may be fixed, but will get reduced marks.
- Source code that would never compile will not be assessed.
- There are no marks available for "sketches" of code.
- Minor typos in filenames (e.g. for graphs) may be ignored as long as the intent is clear
  and it looks like a good-faith attempt was made to follow the spec.
- Graphs in random formats and locations will get reduced marks.

Notes
=====

Executables not getting re-compiled
-----------------------------------

The original makefile did not express that the binaries in `bin`
also depend on the headers. This meant that that `make` did not
consider the target binary out of date, even if the headers were
changed, so it would not get compiled.

This is fixed in the current makefile by making programs depend
on all headers.

When using `make` you can also temporarily fix this by using
the `-B` flag to force it to consider everything out of date.

_Thanks to Fred Harrison and George Orchard for identifying the problem._


Error "No implementation for 'locked' database."
------------------------------------------------

By default the `create_real_time_database` throws an exception if
you instantiate one of the databases to be implemented. This is
mainly intended to help people track what they have or have
not done yet, so that they are not trying to test database
implementations that are empty.

The deliverables section identifies the files that will be assessed,
but does not preclude you from modifying or adding to the code-base.
The only requirement on the code is that the files mentioned in 
the deliverables must stick to the same public APIs.
