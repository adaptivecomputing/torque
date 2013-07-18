PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ --coverage

lib_LTLIBRARIES = libreq_getcred.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_req_getcred

libreq_getcred_la_SOURCES = scaffolding.c ${PROG_ROOT}/req_getcred.c
libreq_getcred_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_req_getcred_SOURCES = test_req_getcred.c

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh

TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
