PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ --coverage

lib_LTLIBRARIES = libattr_recov.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_attr_recov

libattr_recov_la_SOURCES = scaffolding.c ${PROG_ROOT}/attr_recov.c
libattr_recov_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_attr_recov_SOURCES = test_attr_recov.c

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh

TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
