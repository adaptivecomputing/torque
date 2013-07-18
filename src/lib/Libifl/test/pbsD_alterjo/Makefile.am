PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libpbsD_alterjo.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_pbsD_alterjo

libpbsD_alterjo_la_SOURCES = scaffolding.c ${PROG_ROOT}/pbsD_alterjo.c
libpbsD_alterjo_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_pbsD_alterjo_SOURCES = test_pbsD_alterjo.c

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS} 

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
