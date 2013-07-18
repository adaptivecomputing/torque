PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libmomctl.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_momctl

libmomctl_la_SOURCES = scaffolding.c ${PROG_ROOT}/momctl.c
libmomctl_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_momctl_SOURCES = test_momctl.c

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
