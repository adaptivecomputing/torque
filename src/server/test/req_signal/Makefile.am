PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ --coverage

lib_LTLIBRARIES = libreq_signal.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_req_signal

libreq_signal_la_SOURCES = scaffolding.c ${PROG_ROOT}/req_signal.c
libreq_signal_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_req_signal_LDADD = ../../../test/torque_test_lib/libtorque_test.la ../../../test/scaffold_fail/libscaffold_fail.la
test_req_signal_SOURCES = test_req_signal.c ${PROG_ROOT}/../lib/Libnet/get_hostaddr.c

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh

TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
