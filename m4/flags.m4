#	This file is part of Task-Aware GASPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
#
#	Copyright (C) 2018-2020 Barcelona Supercomputing Center (BSC)


AC_DEFUN([AC_CHECK_COMPILER_FLAG],
	[
		
		ac_save_[]_AC_LANG_PREFIX[]FLAGS="$[]_AC_LANG_PREFIX[]FLAGS"
		AC_MSG_CHECKING([if $[]_AC_CC[] $[]_AC_LANG_PREFIX[]FLAGS supports the $1 flag])
		_AC_LANG_PREFIX[]FLAGS="$[]_AC_LANG_PREFIX[]FLAGS $1"
		AC_LINK_IFELSE(
			[AC_LANG_PROGRAM([[]], [[]])],
			[
				AC_MSG_RESULT([yes])
			], [
				AC_MSG_RESULT([no])
				_AC_LANG_PREFIX[]FLAGS="$ac_save_[]_AC_LANG_PREFIX[]FLAGS"
				
				if test x"${USING_MERCURIUM}" = x"yes" ; then
					AC_MSG_CHECKING([if $[]_AC_CC[] $[]_AC_LANG_PREFIX[]FLAGS supports the $1 flag through the preprocessor])
					_AC_LANG_PREFIX[]FLAGS="$[]_AC_LANG_PREFIX[]FLAGS --Wp,$1"
					AC_LINK_IFELSE(
						[AC_LANG_PROGRAM([[]], [[]])],
						[
							AC_MSG_RESULT([yes])
							# Save the flag
							ac_save_[]_AC_LANG_PREFIX[]FLAGS="$[]_AC_LANG_PREFIX[]FLAGS"
						], [
							AC_MSG_RESULT([no])
							_AC_LANG_PREFIX[]FLAGS="$ac_save_[]_AC_LANG_PREFIX[]FLAGS"
						]
					)
					
					AC_MSG_CHECKING([if $[]_AC_CC[] $[]_AC_LANG_PREFIX[]FLAGS supports the $1 flag through the native compiler])
					_AC_LANG_PREFIX[]FLAGS="$[]_AC_LANG_PREFIX[]FLAGS --Wn,$1"
					AC_LINK_IFELSE(
						[AC_LANG_PROGRAM([[]], [[]])],
						[
							AC_MSG_RESULT([yes])
							# Save the flag
							ac_save_[]_AC_LANG_PREFIX[]FLAGS="$[]_AC_LANG_PREFIX[]FLAGS"
						], [
							AC_MSG_RESULT([no])
							_AC_LANG_PREFIX[]FLAGS="$ac_save_[]_AC_LANG_PREFIX[]FLAGS"
						]
					)
					
					AC_MSG_CHECKING([if $[]_AC_CC[] $[]_AC_LANG_PREFIX[]FLAGS supports the $1 flag through the linker])
					_AC_LANG_PREFIX[]FLAGS="$[]_AC_LANG_PREFIX[]FLAGS --Wl,$1"
					AC_LINK_IFELSE(
						[AC_LANG_PROGRAM([[]], [[]])],
						[
							AC_MSG_RESULT([yes])
						], [
							AC_MSG_RESULT([no])
							_AC_LANG_PREFIX[]FLAGS="$ac_save_[]_AC_LANG_PREFIX[]FLAGS"
						]
					)
				fi
			]
		)
		
	]
)

dnl AC_CHECK_EXTRACT_FIRST_COMPILER_FLAG(VARIABLE-NAME, [list of flags])
AC_DEFUN([AC_CHECK_EXTRACT_FIRST_COMPILER_FLAG],
	[
		ac_save2_[]_AC_LANG_PREFIX[]FLAGS="$[]_AC_LANG_PREFIX[]FLAGS"
		for flag in $2 ; do
			AC_CHECK_COMPILER_FLAG([$flag])
			if test x"$ac_save2_[]_AC_LANG_PREFIX[]FLAGS" != x"$[]_AC_LANG_PREFIX[]FLAGS" ; then
				$1[]="$flag"
				break;
			fi
		done
		[]_AC_LANG_PREFIX[]FLAGS="$ac_save2_[]_AC_LANG_PREFIX[]FLAGS"
		AC_SUBST($1)
	]
)

AC_DEFUN([AX_COMPILE_FLAGS], [
	AC_ARG_ENABLE([debug-mode], [AS_HELP_STRING([--enable-debug-mode],
		[Adds compiler debug flags and enables additional internal debugging mechanisms [default=disabled]])])

	AS_IF([test "$enable_debug_mode" = yes],[
		# Debug mode is enabled
		tagaspi_CPPFLAGS=""
		tagaspi_CXXFLAGS="-Wall -Wextra -Wshadow -fvisibility=hidden -O0 -g3"
	],[
		# Debug mode is disabled
		tagaspi_CPPFLAGS="-DNDEBUG"
		tagaspi_CXXFLAGS="-Wall -Wextra -Wshadow -fvisibility=hidden -O3"
	])

	AC_SUBST(tagaspi_CPPFLAGS)
	AC_SUBST(tagaspi_CXXFLAGS)

	# Disable autoconf default compilation flags
	: ${CPPFLAGS=""}
	: ${CXXFLAGS=""}
	: ${CFLAGS=""}
])
