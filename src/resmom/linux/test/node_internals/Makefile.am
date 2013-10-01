PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage -DPBS_MOM
AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage -DPBS_MOM

lib_LTLIBRARIES = libnode_internals.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_node_internals

libnode_internals_la_SOURCES = scaffolding.c ${PROG_ROOT}/node_internals.cpp
libnode_internals_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_node_internals_SOURCES = test_node_internals.c

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = coverage_run.sh *.gcno *.gcda *.gcov core *.lo
