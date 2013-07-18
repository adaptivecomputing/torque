PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libattr_fn_unkn.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_attr_fn_unkn

libattr_fn_unkn_la_SOURCES = scaffolding.c ${PROG_ROOT}/attr_fn_unkn.c
libattr_fn_unkn_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_attr_fn_unkn_SOURCES = test_attr_fn_unkn.c

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
