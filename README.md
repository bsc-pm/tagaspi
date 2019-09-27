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

# Getting Started

This section describes the software requirements of TAGASPI, the building and installation
process and how the library can be used from user applications.


## Software Requirements

The Task-Aware MPI library requires the installation of the following tools and libraries:

* Automake, autoconf, libtool, make and a C and C++ compiler.
* GPI-2 extended with the low-level operations API.
* [Boost](http://boost.org) library version 1.59 or greater.
* [OmpSs-2](https://github.com/bsc-pm/ompss-2-releases) (version 2019.06b or greater). Required
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

