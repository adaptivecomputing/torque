PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libsite_check_u.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_site_check_u

libsite_check_u_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/site_check_u.cpp
libsite_check_u_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_site_check_u_SOURCES = test_site_check_u.cpp

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
