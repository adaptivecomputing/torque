# $Id: Makefile.am,v 1.9 2006/08/28 23:03:26 ciesnik Exp $
# 
#   DRMAA library for Torque/PBS
#   Copyright (C) 2006  Poznan Supercomputing and Networking Center
#                       DSP team <dsp-devel@hedera.man.poznan.pl>
#
#   This library is free software; you can redistribute it and/or
#   modify it under the terms of the GNU Lesser General Public
#   License as published by the Free Software Foundation; either
#   version 2.1 of the License, or (at your option) any later version.
#
#   This library is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#   Lesser General Public License for more details.
#
#   You should have received a copy of the GNU Lesser General Public
#   License along with this library; if not, write to the Free Software
#   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
#


include $(top_srcdir)/buildutils/config.mk

PBS_LIBS = ../../lib/Libpbs/libtorque.la

AM_CXXFLAGS  = @PTHREAD_CFLAGS@

GPERF       = @GPERF@
GPERFFLAGS  = -L C++ --struct-type --readonly-tables --includes 

if DRMAA_BUILD
lib_LTLIBRARIES     = libdrmaa.la
libdrmaa_la_SOURCES = \
 attrib.cpp \
 attrib.h \
 compat.cpp \
 compat.h \
 drmaa_attrib.gperf \
 drmaa_impl.h \
 error.cpp \
 error.h \
 jobs.cpp \
 jobs.h \
 lookup3.cpp \
 lookup3.h \
 pbs_attrib.gperf \
 session.cpp \
 submit.cpp \
 util.cpp \
 wait.cpp
include_HEADERS     = drmaa.h
BUILT_SOURCES       = drmaa_attrib.cpp pbs_attrib.cpp
EXTRA_DIST          = drmaa_attrib.cpp pbs_attrib.cpp
libdrmaa_la_LIBADD  = $(PBS_LIBS) @PTHREAD_LIBS@

.gperf.c:
	base=`basename $< .gperf`; \
	if $(GPERF) $(GPERFFLAGS) --hash-function-name=$${base}_hash \
		--lookup-function-name=$${base}_lookup $< >$@; \
	then exit 0; \
	else rm $@; exit 1; \
	fi

endif
