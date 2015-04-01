 
PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} -I$(PROG_ROOT)/../include --coverage `xml2-config --cflags`
AM_LIBS=`xml2-config --libs`

lib_LTLIBRARIES = libtest_queue_recycler.la

AM_LDFLAGS = @CHECK_LIBS@ $(lib_LTLIBRARIES) $(AM_LIBS)

check_PROGRAMS = test_queue_recycler

libtest_queue_recycler_la_SOURCES = scaffolding.cpp $(PROG_ROOT)/queue_recycler.cpp
libtest_queue_recycler_la_LDFLAGS = @CHECK_LIBS@ $(AM_LIBS) -shared

test_queue_recycler_SOURCES = test_queue_recycler.cpp

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh
TESTS = $(check_PROGRAMS) 

CLEANFILES = *.gcno *.gcda *.gcov_core *.lo
