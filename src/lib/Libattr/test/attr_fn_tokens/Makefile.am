PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libattr_fn_tokens.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_attr_fn_tokens

libattr_fn_tokens_la_SOURCES = scaffolding.c ${PROG_ROOT}/attr_fn_tokens.c
libattr_fn_tokens_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_attr_fn_tokens_SOURCES = test_attr_fn_tokens.c

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
