PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libdisrfcs.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_disrfcs

libdisrfcs_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/disrfcs.cpp
libdisrfcs_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_disrfcs_SOURCES = test_disrfcs.cpp

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
