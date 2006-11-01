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

AM_CFLAGS  = @PTHREAD_CFLAGS@

GPERF       = @GPERF@
GPERFFLAGS  = --language=ANSI-C --struct-type --readonly-tables --includes --pic

lib_LTLIBRARIES     = libdrmaa.la
libdrmaa_la_SOURCES = \
 attrib.c \
 attrib.h \
 compat.c \
 compat.h \
 drmaa_attrib.gperf \
 drmaa_impl.h \
 error.c \
 error.h \
 jobs.c \
 jobs.h \
 lookup3.c \
 lookup3.h \
 pbs_attrib.gperf \
 session.c \
 submit.c \
 util.c \
 wait.c
include_HEADERS     = drmaa.h
BUILT_SOURCES       = drmaa_attrib.c pbs_attrib.c
EXTRA_DIST          = drmaa_attrib.c pbs_attrib.c
libdrmaa_la_LIBADD  = $(PBS_LIBS) @PTHREAD_LIBS@

.gperf.c:
	base=`basename $< .gperf`; \
	if $(GPERF) $(GPERFFLAGS) --hash-function-name=$${base}_hash \
		--lookup-function-name=$${base}_lookup $< >$@; \
	then exit 0; \
	else rm $@; exit 1; \
	fi

