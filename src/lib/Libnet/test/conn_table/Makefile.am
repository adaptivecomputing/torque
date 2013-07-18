PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libconn_table.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_conn_table

libconn_table_la_SOURCES = scaffolding.c ${PROG_ROOT}/conn_table.c
libconn_table_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_conn_table_SOURCES = test_conn_table.c

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
