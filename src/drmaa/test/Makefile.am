# $Id: Makefile.am,v 1.4 2006/06/08 01:27:17 ciesnik Exp $
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

AM_CPPFLAGS = -I$(top_srcdir)/src/drmaa/src

noinst_PROGRAMS = test example sleeper_job worker_job suicide_job exit_job
test_SOURCES        = test.c ../src/drmaa.h
example_SOURCES     = example.c ../src/drmaa.h
sleeper_job_SOURCES = sleeper_job.c
worker_job_SOURCES  = worker_job.c
suicide_job_SOURCES = suicide_job.c
exit_job_SOURCES    = exit_job.c
test_LDADD          = @top_builddir@/src/drmaa/src/libdrmaa.la
example_LDADD       = @top_builddir@/src/drmaa/src/libdrmaa.la

