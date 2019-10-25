ACLOCAL_AMFLAGS=-I m4

EXTRA_DIST=\
 src/common/*.h\
 src/include/*.h

AM_CPPFLAGS=\
 @gaspiinc@ \
 $(BOOST_CPPFLAGS) -DBOOST_ENABLE_ASSERT_DEBUG_HANDLER \
 -I@top_srcdir@/src \
 -I@top_srcdir@/src/include \
 -include "config.h" \
 $(CPPFLAGS)

if debug_mode_enabled
AM_CXXFLAGS=-O0 -g3 -std=c++11 -Wall -Wextra $(CXXFLAGS)
else
AM_CXXFLAGS=-O3 -std=c++11 -fno-exceptions -Wall -Wextra $(CXXFLAGS)
endif
AM_CXXFLAGS += $(FALIGNED_NEW_FLAG)

AM_LDFLAGS=$(BOOST_LDFLAGS) -ldl $(LDFLAGS)
LIBS=

include_HEADERS= src/include/TAGASPI.h
pkginclude_HEADERS= # This library does not provide any additional header

c_api_sources=               \
 src/c/Initialization.cpp    \
 src/c/Write.cpp             \
 src/c/Read.cpp              \
 src/c/Notify.cpp            \
 src/c/WriteNotify.cpp       \
 src/c/WriteList.cpp         \
 src/c/ReadList.cpp          \
 src/c/WriteListNotify.cpp   \
 src/c/AsyncWaitallReset.cpp \
 src/c/QueueGroups.cpp

fortran_api_sources=

common_sources=              \
 src/common/Allocator.cpp    \
 src/common/Environment.cpp  \
 src/common/Polling.cpp      \
 src/common/TaskingModel.cpp

noinst_HEADERS =                       \
 src/common/Allocator.hpp              \
 src/common/Environment.hpp            \
 src/common/HardwareInfo.hpp           \
 src/common/Polling.hpp                \
 src/common/QueueGroup.hpp             \
 src/common/TaskingModelAPI.hpp        \
 src/common/TaskingModel.hpp           \
 src/common/WaitingRange.hpp           \
 src/common/WaitingRangeList.hpp       \
 src/common/WaitingRangeQueue.hpp      \
 src/common/util/MPSCLockFreeQueue.hpp \
 src/common/util/SpinLock.hpp          \
 src/common/util/SymbolResolver.hpp    \
 src/common/util/Utils.hpp

lib_LTLIBRARIES=libtagaspi.la libtagaspi-c.la
libtagaspi_la_SOURCES=$(common_sources) $(c_api_sources) $(fortran_api_sources)
libtagaspi_c_la_SOURCES=$(common_sources) $(c_api_sources)
#libtagaspi_fortran_la_SOURCES=$(common_sources) $(fortran_api_sources)

