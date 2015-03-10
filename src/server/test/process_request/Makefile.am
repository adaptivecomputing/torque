PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ --coverage

lib_LTLIBRARIES = libprocess_request.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_process_request

libprocess_request_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/process_request.cpp
libprocess_request_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_process_request_LDADD = ../../../test/torque_test_lib/libtorque_test.la ../../../test/scaffold_fail/libscaffold_fail.la
test_process_request_SOURCES = test_process_request.cpp ${PROG_ROOT}/../lib/Libnet/get_hostaddr.cpp

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh

TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
