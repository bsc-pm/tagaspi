# Task-Aware GASPI Library

The Task-Aware GASPI or TAGASPI library extends the functionality of the standard GASPI
library by providing new mechanisms for improving the interoperability between parallel
task-based programming models, such as OpenMP or OmpSs-2, and RDMA GASPI operations. The
TAGASPI approach targets hybrid applications that taskify the RDMA communications. In this
way, communication tasks can run in parallel, and also, they can overlap with computation
tasks.

TAGASPI is currently compatible with the [OmpSs-2](https://github.com/bsc-pm/ompss-2-releases)
task-based programming model. However, in the near future, it is going to be compatible
with a derived implementation of OpenMP.

## References

More information about this work can be found in the article below. **Citations to the TAGASPI library should reference this article**:

* Sala, K., Macia, S., & Beltran, V. (2021, September). **Combining One-Sided Communications with Task-Based Programming Models**. In 2021 IEEE International Conference on Cluster Computing (CLUSTER) (pp. 528-541). IEEE. [[Article link]](https://doi.org/10.1109/Cluster48925.2021.00024)

## Acknowledgments

This work was financially supported by the PRACE project funded in part by the EU’s Horizon 2020
Research programme (2014-2020) under grant agreement 823767.

# Getting Started

This section describes the software requirements of TAGASPI, the building and installation
process and how the library can be used from user applications.

## Software Requirements

The Task-Aware MPI library requires the installation of the following tools and libraries:

* Automake, autoconf, libtool, make and a C and C++ compiler.
* [GPI-2](https://pm.bsc.es/gitlab/interoperability/extern/GPI-2) (tag `tagaspi-2021.11`) which extends
  the GASPI interface with the low-level operations API. We recommend configuring GPI-2 with MPI
  support in order to use `mpirun` or `srun` for launching hybrid applications. The GPI-2 version
  that supports the current TAGASPI version is the one included in the tag `tagaspi-2021.11`.
* [Boost](http://boost.org) library version 1.59 or greater.
* [OmpSs-2](https://github.com/bsc-pm/ompss-2-releases) (version 2018.11 or greater). Required
  when compiling the hybrid applications.

## Building and Installing

TAGASPI uses the standard GNU automake and libtool toolchain. When cloning from a repository, the
building environment must be prepared through the following command:

```bash
$ autoreconf -fiv
```

When the code is distributed through a tarball, it usually does not need that command.

Then execute the following commands:

```bash
$ ./configure --prefix=$INSTALLATION_PREFIX --with-gaspi=$GPI2_HOME --with-boost=$BOOST_HOME ..other options..
$ make
$ make install
```

where `$INSTALLATION_PREFIX` is the directory into which to install TAGASPI, `$GPI2_HOME` is the prefix
of the extended GPI-2 installation, and `$BOOST_HOME` is the prefix of the Boost installation.

Other optional configuration flags are:
* `--enable-debug-mode`: Adds compiler debug flags and enables additional internal debugging mechanisms.
   Note that this flag can downgrade the overall performance. Debug mode is **disabled** by default.

Once TAGASPI is built and installed, e.g, in `$TAGASPI_HOME`, the installation folder will contain the
libraries in `$TAGASPI_HOME/lib` (or `$TAGASPI_HOME/lib64`) and the headers in `$TAGASPI_HOME/include`.
At this moment, TAGASPI is supporting C/C++ applications, but we are working on providing the Fortran
support. The building procedure installs a single `libtagaspi` library (both in a dynamic and static
format). There is a header file which can be included from user applications that declare all needed
functions and constants: `TAGASPI.h`.

## Run-time Options

The TAGASPI library has some run-time options that can be set through environment variables. These are:

* `TAGASPI_POLLING_FREQUENCY` (default `100` us): The polling frequency in which the in-flight one-sided
TAGASPI operations are checked by a transparent task in the background. This polling task is scheduled
every specific number of microseconds to check the pending operations that have been issued by the tasks
that called the TAGASPI communication services. This envar expects a positive integer number; the value `0`
means that the task should be always running. Notice that the task leverages a CPU while running. By default,
this task runs every `100` microseconds, but this value may be too high in communication-intensive applications.
This envar is only considered when running with OmpSs-2 2020.06 or newer.

## Using TAGASPI in Hybrid Applications

User applications should be linked against the GPI-2 library, the OmpSs-2's runtime system and the TAGASPI
library. Assuming that GPI-2 was built with MPI interoperability, a hybrid OmpSs-2+GASPI application can be
compiled and linked as follows:

```bash
$ mpicxx -cxx=mcxx --ompss-2 -I$TAGASPI_HOME/include -I$GPI2_HOME/include app.cpp -o app.bin -ltagaspi -L$TAGASPI_HOME/lib -lGPI2 -L$GPI2_HOME/lib
```

Finally, the application binary can be launched as a regular GASPI/MPI program:

```bash
$ mpirun -n 4 ...binding options... ./app.bin
```

