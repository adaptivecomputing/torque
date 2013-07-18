PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ --coverage

lib_LTLIBRARIES = libreq_rescq.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_req_rescq

libreq_rescq_la_SOURCES = scaffolding.c ${PROG_ROOT}/req_rescq.c
libreq_rescq_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_req_rescq_SOURCES = test_req_rescq.c

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh

TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
