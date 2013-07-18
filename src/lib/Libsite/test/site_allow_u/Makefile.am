PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libsite_allow_u.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_site_allow_u

libsite_allow_u_la_SOURCES = scaffolding.c ${PROG_ROOT}/site_allow_u.c
libsite_allow_u_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_site_allow_u_SOURCES = test_site_allow_u.c

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
