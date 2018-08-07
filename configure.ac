#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([Task-aware GASPI library], [0.1], [pm-tools@bsc.es])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src])
AC_CONFIG_HEADERS([config.h])

AC_LANG([C])

# Check if debug mode is enabled
AX_DEBUG_MODE

# Look for a valid C compiler
AC_PROG_CC

# AM_PROG_AR must be called before LT_INIT or a warning ensues
AM_PROG_AR

# Automake initialization
AM_INIT_AUTOMAKE([foreign -Wall dist-bzip2 -Wno-portability subdir-objects silent-rules])
AM_SILENT_RULES([yes])

# Checks for header files.
AC_CHECK_HEADERS([unistd.h])
AC_CHECK_HEADERS([stdbool.h])
AC_CHECK_HEADERS([xmmintrin.h])

# Check for GASPI
AX_CHECK_GASPI

# Check for EXTRAE
AX_CHECK_EXTRAE

# Checks for typedefs, structures, and compiler characteristics
AC_TYPE_SIZE_T

# Turn off shared libraries.
LT_INIT([static])

# Enable/disable architecture specific code
AS_CASE([$host],
  [x86_64*],[ARCH=x86_64])
AS_IF([test "$ARCH" = "x86_64"],[
  AC_DEFINE([X86_64_ARCH],[1],[Compiling for x86_64 host])
])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

echo "- Task-aware GASPI library -"
echo "       - Configuration summary -"
echo "Installation prefix: $prefix"
