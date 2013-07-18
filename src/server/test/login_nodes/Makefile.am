
PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I$(PROG_ROOT)/../include --coverage

lib_LTLIBRARIES = libtest_login_nodes.la

AM_LDFLAGS = @CHECK_LIBS@ $(lib_LTLIBRARIES)

check_PROGRAMS = test_login_nodes

libtest_login_nodes_la_SOURCES = scaffolding.c $(PROG_ROOT)/login_nodes.c $(PROG_ROOT)/execution_slot_tracker.cpp
libtest_login_nodes_la_LDFLAGS = @CHECK_LIBS@ $(AM_LIBS) -shared -lgcov

test_login_nodes_SOURCES = test_login_nodes.c

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh

TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
