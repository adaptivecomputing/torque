PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage -DPBSPD=\"$(bindir)/pbspd\"

lib_LTLIBRARIES = libpbspoe.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_pbspoe

libpbspoe_la_SOURCES = scaffolding.c ${PROG_ROOT}/pbspoe.c
libpbspoe_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_pbspoe_SOURCES = test_pbspoe.c

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh

TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
