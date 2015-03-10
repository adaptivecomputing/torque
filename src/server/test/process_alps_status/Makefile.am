
PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I$(PROG_ROOT)/../include --coverage `xml2-config --cflags` 
AM_LIBS=`xml2-config --libs`

lib_LTLIBRARIES = libtest_process_alps_status.la

AM_LDFLAGS = @CHECK_LIBS@ $(lib_LTLIBRARIES) $(AM_LIBS)

check_PROGRAMS = test_process_alps_status

libtest_process_alps_status_la_SOURCES = scaffolding.cpp $(PROG_ROOT)/process_alps_status.cpp $(PROG_ROOT)/../lib/Libutils/u_constants.cpp $(PROG_ROOT)/job_attr_def.cpp
libtest_process_alps_status_la_LDFLAGS = @CHECK_LIBS@ $(AM_LIBS) -shared -lgcov

test_process_alps_status_LDADD = ../../../test/torque_test_lib/libtorque_test.la ../../../test/scaffold_fail/libscaffold_fail.la
test_process_alps_status_SOURCES = test_process_alps_status.cpp

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh

TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov_core *.lo

