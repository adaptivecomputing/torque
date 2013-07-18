PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libdiswsi.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_diswsi

libdiswsi_la_SOURCES = scaffolding.c ${PROG_ROOT}/diswsi.c
libdiswsi_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_diswsi_SOURCES = test_diswsi.c

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
