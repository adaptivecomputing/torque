PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libpbsdsh.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_pbsdsh

libpbsdsh_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/pbsdsh.cpp
libpbsdsh_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_pbsdsh_SOURCES = test_pbsdsh.cpp

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh

TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
