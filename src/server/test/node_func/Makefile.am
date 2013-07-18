PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ --coverage

lib_LTLIBRARIES = libnode_func.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_node_func

libnode_func_la_SOURCES = scaffolding.c ${PROG_ROOT}/node_func.c ${PROG_ROOT}/execution_slot_tracker.cpp
libnode_func_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_node_func_SOURCES = test_node_func.c

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh

TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
