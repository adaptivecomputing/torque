 
PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} -I$(PROG_ROOT)/../include --coverage `xml2-config --cflags`
AM_LIBS=`xml2-config --libs`

lib_LTLIBRARIES = libtest_delete_all_tracker.la

AM_LDFLAGS = @CHECK_LIBS@ $(lib_LTLIBRARIES) $(AM_LIBS)

check_PROGRAMS = test_delete_all_tracker

libtest_delete_all_tracker_la_SOURCES = scaffolding.c $(PROG_ROOT)/delete_all_tracker.cpp
libtest_delete_all_tracker_la_LDFLAGS = @CHECK_LIBS@ $(AM_LIBS) -shared

test_delete_all_tracker_SOURCES = test_delete_all_tracker.c

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh
TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov_core *.lo
