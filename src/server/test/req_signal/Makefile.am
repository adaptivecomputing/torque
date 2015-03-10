PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ --coverage

lib_LTLIBRARIES = libreq_signal.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_req_signal

libreq_signal_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/req_signal.cpp
libreq_signal_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_req_signal_LDADD = ../../../test/torque_test_lib/libtorque_test.la ../../../test/scaffold_fail/libscaffold_fail.la
test_req_signal_SOURCES = test_req_signal.cpp ${PROG_ROOT}/../lib/Libnet/get_hostaddr.cpp

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh

TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
