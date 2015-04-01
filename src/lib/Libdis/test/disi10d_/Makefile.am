PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libdisi10d_.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_disi10d_

libdisi10d__la_SOURCES = scaffolding.cpp ${PROG_ROOT}/disi10d_.cpp
libdisi10d__la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_disi10d__SOURCES = test_disi10d_.cpp

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
