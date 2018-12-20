#
# SYNOPSIS
#
#   AX_RUNTIME_CPU_INFO
#

AC_DEFUN([AX_RUNTIME_CPU_INFO], [
	AC_MSG_CHECKING([whether the runtime API to get CPU info is available])
	AC_ARG_ENABLE(
		[runtime-cpu-info],
		[AS_HELP_STRING([--disable-runtime-cpu-info], [do not use the runtime API to get CPU information [default=enabled]])],
		[
			case "${enableval}" in
			yes)
				AC_MSG_RESULT([yes])
				ac_runtime_cpu_info=yes
				;;
			no)
				AC_MSG_RESULT([no])
				ac_runtime_cpu_info=no
				;;
			*)
				AC_MSG_ERROR([bad value ${enableval} for --disable-runtime-cpu-info])
				;;
			esac
		],
		[
			AC_MSG_RESULT([yes])
			ac_runtime_cpu_info=yes
		]
	)
	if test x"${ac_runtime_cpu_info}" = x"no" ; then
		AC_DEFINE([HAVE_RUNTIME_CPU_INFO], 0, [Use the runtime API to get CPU information])
	else
		AC_DEFINE([HAVE_RUNTIME_CPU_INFO], 1, [Use the runtime API to get information])
	fi
])
