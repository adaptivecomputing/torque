PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libattr_fn_intr.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_attr_fn_intr

libattr_fn_intr_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/attr_fn_intr.cpp
libattr_fn_intr_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_attr_fn_intr_SOURCES = test_attr_fn_intr.cpp

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
