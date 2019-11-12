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
            gaspi_request_wait],
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

