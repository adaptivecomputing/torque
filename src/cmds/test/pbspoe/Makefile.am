PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage -DPBSPD=\"$(bindir)/pbspd\"

lib_LTLIBRARIES = libpbspoe.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_pbspoe

libpbspoe_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/pbspoe.cpp
libpbspoe_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_pbspoe_SOURCES = test_pbspoe.cpp

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh

TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
