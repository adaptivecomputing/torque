PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libparse_equal.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_parse_equal

libparse_equal_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/parse_equal.cpp
libparse_equal_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_parse_equal_SOURCES = test_parse_equal.cpp

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
