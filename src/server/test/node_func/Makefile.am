PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ --coverage `xml2-config --cflags`
AM_LIBS=`xml2-config --libs`

lib_LTLIBRARIES = libnode_func.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_node_func

libnode_func_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/node_func.cpp ${PROG_ROOT}/execution_slot_tracker.cpp
libnode_func_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_node_func_SOURCES = test_node_func.cpp

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh

TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
