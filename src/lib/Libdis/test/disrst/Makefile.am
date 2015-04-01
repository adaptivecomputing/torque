PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libdisrst.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_disrst

libdisrst_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/disrst.cpp
libdisrst_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_disrst_SOURCES = test_disrst.cpp

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
