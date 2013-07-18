PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libpbsD_holdjob.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_pbsD_holdjob

libpbsD_holdjob_la_SOURCES = scaffolding.c ${PROG_ROOT}/pbsD_holdjob.c
libpbsD_holdjob_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_pbsD_holdjob_SOURCES = test_pbsD_holdjob.c

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS} 

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
