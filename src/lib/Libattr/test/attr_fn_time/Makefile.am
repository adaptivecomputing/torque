PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libattr_fn_time.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_attr_fn_time

libattr_fn_time_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/attr_fn_time.cpp
libattr_fn_time_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_attr_fn_time_SOURCES = test_attr_fn_time.cpp

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
