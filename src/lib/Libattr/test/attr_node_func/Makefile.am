PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libattr_node_func.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_attr_node_func

libattr_node_func_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/attr_node_func.cpp
libattr_node_func_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_attr_node_func_SOURCES = test_attr_node_func.cpp
test_attr_node_func_LDADD = ../../../../test/torque_test_lib/libtorque_test.la ../../../../test/scaffold_fail/libscaffold_fail.la

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
