PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libqdisable.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_qdisable

libqdisable_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/qdisable.cpp
libqdisable_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_qdisable_SOURCES = test_qdisable.cpp

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh

TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
