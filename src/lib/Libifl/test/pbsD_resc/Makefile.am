PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libpbsD_resc.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_pbsD_resc

libpbsD_resc_la_SOURCES = scaffolding.c ${PROG_ROOT}/pbsD_resc.c
libpbsD_resc_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_pbsD_resc_SOURCES = test_pbsD_resc.c

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS} 

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
