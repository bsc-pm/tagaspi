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
AX_CXX_COMPILE_STDCXX_11([], [mandatory])

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

AC_CHECK_EXTRACT_FIRST_COMPILER_FLAG([FALIGNED_NEW_FLAG], [-faligned-new])
AX_APPEND_FLAG([${FALIGNED_NEW_FLAG}], [tagaspi_CXXFLAGS])
AC_SUBST(tagaspi_CXXFLAGS)

AX_CHECK_CPUS

LT_INIT([shared static pic-only])

# Enable/disable architecture specific code
AS_CASE([$host],
  [x86_64*],[ARCH=x86_64])
AS_IF([test "$ARCH" = "x86_64"],[
  AC_DEFINE([X86_64_ARCH],[1],[Compiling for x86_64 host])
])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

echo ""
echo ""
echo "Configuration summary:"
echo "    Installation prefix... ${prefix}"
echo ""
echo "    CXXFLAGS... ${tagaspi_CXXFLAGS} ${CXXFLAGS}"
echo "    CPPFLAGS... ${tagaspi_CPPFLAGS} ${CPPFLAGS}"
echo ""
echo "    GASPI CPPFLAGS... ${gaspiinc}"
echo ""
echo "    Boost CPPFLAGS... ${BOOST_CPPFLAGS}"
echo "    Boost LDFLAGS... ${BOOST_LDFLAGS}"
echo ""
echo "    libnuma CPPFLAGS... ${libnuma_CPPFLAGS}"
echo "    libnuma LIBS... ${libnuma_LIBS}"
