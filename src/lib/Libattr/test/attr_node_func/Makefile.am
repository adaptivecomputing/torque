PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libattr_node_func.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_attr_node_func

libattr_node_func_la_SOURCES = ${PROG_ROOT}/attr_node_func.c scaffolding.c
libattr_node_func_la_LDFLAGS = @CHECK_LIBS@ -lgcov

test_attr_node_func_LDADD = ../../../../test/torque_test_lib/libtorque_test.la ../../../../test/scaffold_fail/libscaffold_fail.la
test_attr_node_func_SOURCES = test_attr_node_func.c

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
