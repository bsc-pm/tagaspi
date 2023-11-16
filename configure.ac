#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

#	This file is part of Task-Aware GASPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
#
#	Copyright (C) 2018-2021 Barcelona Supercomputing Center (BSC)

AC_PREREQ([2.69])
AC_INIT([Task-aware GASPI library], [2021.11], [pm-tools@bsc.es])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src])
AC_CONFIG_HEADERS([config.h])

AC_LANG([C++])

# Prepare compilation flags before AC_PROG_CXX
AX_COMPILE_FLAGS

# Look for a valid C++ compiler
AC_PROG_CXX
AC_PROG_FC

# Retrieve compiler version
AX_CHECK_CXX_VERSION

# AM_PROG_AR must be called before LT_INIT or a warning ensues
AM_PROG_AR

# Automake initialization
AM_INIT_AUTOMAKE([foreign -Wall dist-bzip2 -Wno-portability subdir-objects silent-rules])
AM_SILENT_RULES([yes])

# Checks for header files.
AC_CHECK_HEADERS([unistd.h])
AC_CHECK_HEADERS([xmmintrin.h])

# Check libraries
AC_CHECK_LIBNUMA

# Check for GASPI
AX_CHECK_GASPI

# Check for Boost
AX_BOOST_BASE([1.59], [], [ AC_MSG_ERROR([Boost library not found])])

# Checks for typedefs, structures, and compiler characteristics
AC_TYPE_SIZE_T

# Check that the compiler supports C++17
AX_CXX_COMPILE_STDCXX_17([noext], [mandatory])

# Check the CPU count of the system
AX_CHECK_CPUS

LT_INIT([shared static pic-only])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

echo ""
echo ""
echo "Configuration summary:"
echo "    Compiler version... ${CXX_VERSION}"
echo ""
echo "    Installation prefix... ${prefix}"
echo ""
echo "    CXXFLAGS... ${tagaspi_CXXFLAGS} ${CXXFLAGS}"
echo "    CPPFLAGS... ${tagaspi_CPPFLAGS} ${CPPFLAGS}"
echo "    LDFLAGS... ${LDFLAGS}"
echo ""
echo "    GASPI CPPFLAGS... ${gaspiinc}"
echo ""
echo "    Boost CPPFLAGS... ${BOOST_CPPFLAGS}"
echo ""
echo "    libnuma CPPFLAGS... ${libnuma_CPPFLAGS}"
echo "    libnuma LIBS... ${libnuma_LIBS}"
