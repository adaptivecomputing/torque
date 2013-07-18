PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libdisruc.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_disruc

libdisruc_la_SOURCES = scaffolding.c ${PROG_ROOT}/disruc.c
libdisruc_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_disruc_SOURCES = test_disruc.c

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
