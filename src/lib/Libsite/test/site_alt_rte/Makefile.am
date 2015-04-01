PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libsite_alt_rte.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_site_alt_rte

libsite_alt_rte_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/site_alt_rte.cpp
libsite_alt_rte_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_site_alt_rte_SOURCES = test_site_alt_rte.cpp

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
