PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libPBS_attr.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_PBS_attr

libPBS_attr_la_SOURCES = scaffolding.c ${PROG_ROOT}/PBS_attr.c
libPBS_attr_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_PBS_attr_SOURCES = test_PBS_attr.c

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS} 

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
