PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libqgpureset.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_qgpureset

libqgpureset_la_SOURCES = scaffolding.c ${PROG_ROOT}/qgpureset.c
libqgpureset_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_qgpureset_SOURCES = test_qgpureset.c

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh

TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
