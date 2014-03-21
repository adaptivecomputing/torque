 
PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} -I$(PROG_ROOT)/../include --coverage `xml2-config --cflags`
AM_LIBS=`xml2-config --libs`

lib_LTLIBRARIES = libtest_job_usage_info.la

AM_LDFLAGS = @CHECK_LIBS@ $(lib_LTLIBRARIES) $(AM_LIBS)

check_PROGRAMS = test_job_usage_info

libtest_job_usage_info_la_SOURCES = scaffolding.c $(PROG_ROOT)/job_usage_info.cpp
libtest_job_usage_info_la_LDFLAGS = @CHECK_LIBS@ $(AM_LIBS) -shared -lgcov

test_job_usage_info_SOURCES = test_job_usage_info.c

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh

TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
