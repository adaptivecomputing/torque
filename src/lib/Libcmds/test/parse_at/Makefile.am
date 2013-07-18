PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libparse_at.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_parse_at

libparse_at_la_SOURCES = scaffolding.c ${PROG_ROOT}/parse_at.c
libparse_at_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_parse_at_SOURCES = test_parse_at.c

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
