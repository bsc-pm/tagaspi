#
# SYNOPSIS
#
#   AX_CHECK_GASPI
#
# DESCRIPTION
#
#   Check whether GASPI path to the headers and libraries are correctly specified.
#
# LICENSE
#
#   Copyright (c) 2015 Jorge Bellon <jbellon@bsc.es>
#
#   This program is free software: you can redistribute it and/or modify it
#   under the terms of the GNU General Public License as published by the
#   Free Software Foundation, either version 3 of the License, or (at your
#   option) any later version.
#
#   This program is distributed in the hope that it will be useful, but
#   WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
#   Public License for more details.
#
#   You should have received a copy of the GNU General Public License along
#   with this program. If not, see <http://www.gnu.org/licenses/>.
#
#   As a special exception, the respective Autoconf Macro's copyright owner
#   gives unlimited permission to copy, distribute and modify the configure
#   scripts that are the output of Autoconf when processing the Macro. You
#   need not follow the terms of the GNU General Public License when using
#   or distributing such scripts, even though portions of the text of the
#   Macro appear in them. The GNU General Public License (GPL) does govern
#   all other use of the material that constitutes the Autoconf Macro.
#
#   This special exception to the GPL applies to versions of the Autoconf
#   Macro released by the Autoconf Archive. When you make and distribute a
#   modified version of the Autoconf Macro, you may extend this special
#   exception to the GPL to apply to your modified version as well.

AC_DEFUN([AX_CHECK_GASPI],[

#Check if a GASPI implementation is installed.
AC_ARG_WITH(gaspi,
[AS_HELP_STRING([--with-gaspi,--with-gaspi=PATH],
                [search in system directories or specify prefix directory for installed GASPI package])])
AC_ARG_WITH(gaspi-include,
[AS_HELP_STRING([--with-gaspi-include=PATH],
                [specify directory for installed GASPI include files])])
AC_ARG_WITH(gaspi-lib,
[AS_HELP_STRING([--with-gaspi-lib=PATH],
                [specify directory for the installed GASPI library])])

# Search for GASPI by default
AS_IF([test "$with_gaspi" != yes],[
  gaspiinc="-I$with_gaspi/include"
  AS_IF([test -d $with_gaspi/lib64],
    [olibdir=$with_gaspi/lib64],
    [olibdir=$with_gaspi/lib])

  gaspilib="-L$olibdir -Wl,-rpath,$olibdir"
])

AS_IF([test "x$with_gaspi_include" != x],[
  gaspiinc="-I$with_gaspi_include"
])
AS_IF([test "x$with_gaspi_lib" != x],[
  gaspilib="-L$with_gaspi_lib -Wl,-rpath,$with_gaspi_lib"
])

# Tests if provided headers and libraries are usable and correct
AX_VAR_PUSHVALUE([CPPFLAGS],[$CPPFLAGS $gaspiinc])
AX_VAR_PUSHVALUE([CFLAGS])
AX_VAR_PUSHVALUE([LDFLAGS],[$LDFLAGS $gaspilib])
AX_VAR_PUSHVALUE([LIBS],[])

AC_MSG_CHECKING([which implementation to target])
  AC_MSG_RESULT([GPI2])
  AC_CHECK_HEADERS([GASPI.h], [gaspi=yes; break], [gaspi=no])
  AS_IF([test "$gaspi" = "yes"],[
    AC_CHECK_HEADERS([GASPI_Lowlevel.h], [gaspi=yes; break], [gaspi=no])
  ])
  search_lib=GPI2
  required_libs="-lpthread -libverbs"


m4_foreach([function],
           [gaspi_operation_submit,
            gaspi_operation_list_submit,
            gaspi_operation_get_num_requests,
            gaspi_request_wait,
            gaspi_request_get_tag,
            gaspi_request_get_status],
           [
             AS_IF([test "$gaspi" = "yes"],[
               AC_SEARCH_LIBS(function,
                              [$search_lib],
                              [gaspi=yes],
                              [gaspi=no],
                              [$required_libs])dnl
             ])
           ])dnl

gaspilibs=$LIBS

AX_VAR_POPVALUE([CPPFLAGS])
AX_VAR_POPVALUE([CFLAGS])
AX_VAR_POPVALUE([LDFLAGS])
AX_VAR_POPVALUE([LIBS])

AS_IF([test "$gaspi" != "yes"],[
    AC_MSG_ERROR([
------------------------------
GASPI path was not correctly specified. 
Please, check that the provided directories are correct.
------------------------------])
])

AC_SUBST([gaspi])
AC_SUBST([gaspiinc])
AC_SUBST([gaspilib])
AC_SUBST([gaspilibs])

])dnl AX_CHECK_GASPI

