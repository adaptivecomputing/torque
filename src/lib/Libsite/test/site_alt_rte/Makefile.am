PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libsite_alt_rte.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_site_alt_rte

libsite_alt_rte_la_SOURCES = scaffolding.c ${PROG_ROOT}/site_alt_rte.c
libsite_alt_rte_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_site_alt_rte_SOURCES = test_site_alt_rte.c

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
