PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libdiswsl.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_diswsl

libdiswsl_la_SOURCES = scaffolding.c ${PROG_ROOT}/diswsl.c
libdiswsl_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_diswsl_SOURCES = test_diswsl.c

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
