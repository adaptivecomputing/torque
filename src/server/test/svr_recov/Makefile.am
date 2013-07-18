PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ --coverage

lib_LTLIBRARIES = libsvr_recov.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_svr_recov

libsvr_recov_la_SOURCES = scaffolding.c ${PROG_ROOT}/svr_recov.c
libsvr_recov_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_svr_recov_SOURCES = test_svr_recov.c

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh

TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
