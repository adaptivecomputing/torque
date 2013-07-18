PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libdisrsi_.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_disrsi_

libdisrsi__la_SOURCES = scaffolding.c ${PROG_ROOT}/disrsi_.c
libdisrsi__la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_disrsi__SOURCES = test_disrsi_.c

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
