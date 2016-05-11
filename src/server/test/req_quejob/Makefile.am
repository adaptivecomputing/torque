PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -DUT_REQ_QUEJOB -I${PROG_ROOT}/ --coverage

lib_LTLIBRARIES = libreq_quejob.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_req_quejob

libreq_quejob_la_SOURCES = scaffolding.c ${PROG_ROOT}/req_quejob.c
libreq_quejob_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_req_quejob_LDADD = ../../../test/torque_test_lib/libtorque_test.la ../../../test/scaffold_fail/libscaffold_fail.la
test_req_quejob_SOURCES = test_req_quejob.c

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh

TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
