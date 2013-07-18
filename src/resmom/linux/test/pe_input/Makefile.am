PROG_ROOT = ../..

AM_CFLAGS = -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libpe_input.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_pe_input

libpe_input_la_SOURCES = scaffolding.c ${PROG_ROOT}/pe_input.c
libpe_input_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_pe_input_SOURCES = test_pe_input.c

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
