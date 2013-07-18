PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libdisrcs.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_disrcs

libdisrcs_la_SOURCES = scaffolding.c ${PROG_ROOT}/disrcs.c
libdisrcs_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_disrcs_SOURCES = test_disrcs.c

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
