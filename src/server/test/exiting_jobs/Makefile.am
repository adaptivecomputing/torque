 
PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I$(PROG_ROOT)/../include --coverage `xml2-config --cflags`
AM_LIBS=`xml2-config --libs`

lib_LTLIBRARIES = libtest_exiting_jobs.la

AM_LDFLAGS = @CHECK_LIBS@ $(lib_LTLIBRARIES) $(AM_LIBS)

check_PROGRAMS = test_exiting_jobs

libtest_exiting_jobs_la_SOURCES = scaffolding.cpp $(PROG_ROOT)/exiting_jobs.cpp
libtest_exiting_jobs_la_LDFLAGS = @CHECK_LIBS@ $(AM_LIBS) -shared -lgcov

test_exiting_jobs_LDADD = ../../../test/torque_test_lib/libtorque_test.la ../../../test/scaffold_fail/libscaffold_fail.la
test_exiting_jobs_SOURCES = test_exiting_jobs.cpp

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh

TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
