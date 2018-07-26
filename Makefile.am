ACLOCAL_AMFLAGS=-I m4

EXTRA_DIST=\
 src/common/*.h\
 src/include/*.h

AM_CPPFLAGS=\
 @gaspiinc@ \
 @extraeinc@ \
 -I@top_srcdir@/src \
 -I@top_srcdir@/src/common \
 -include "config.h" \
 $(CPPFLAGS)

if debug_mode_enabled
AM_CFLAGS=-O0 -g3 -Wall -Wextra $(CFLAGS)
else
AM_CFLAGS=-O3 -fno-exceptions -Wall -Wextra $(CFLAGS)
endif

AM_LDFLAGS=@extraelib@ -ldl $(LDFLAGS)
LIBS=@extraelibs@

include_HEADERS= src/include/TAGASPI.h
pkginclude_HEADERS= # This library does not provide any additional header

c_api_sources=
fortran_api_sources=
common_sources=

lib_LTLIBRARIES=
libtagaspi_la_SOURCES=$(common_sources) $(c_api_sources) $(fortran_api_sources)
libtagaspi_c_la_SOURCES=$(common_sources) $(c_api_sources)
libtagaspi_fortran_la_SOURCES=$(common_sources) $(fortran_api_sources)

