PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libconn_table.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_conn_table

libconn_table_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/conn_table.cpp
libconn_table_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_conn_table_SOURCES = test_conn_table.cpp

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
