PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libpbsD_locjob.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_pbsD_locjob

libpbsD_locjob_la_SOURCES = scaffolding.c ${PROG_ROOT}/pbsD_locjob.c
libpbsD_locjob_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_pbsD_locjob_SOURCES = test_pbsD_locjob.c

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS} 

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
