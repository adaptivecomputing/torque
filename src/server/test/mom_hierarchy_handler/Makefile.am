 
PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I$(PROG_ROOT)/../include --coverage 
AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/../include --coverage 

lib_LTLIBRARIES = libtest_mom_hierarchy_handler.la

AM_LDFLAGS = @CHECK_LIBS@ $(lib_LTLIBRARIES)

check_PROGRAMS = test_mom_hierarchy_handler

libtest_mom_hierarchy_handler_la_SOURCES = scaffolding.c ${PROG_ROOT}/mom_hierarchy_handler.cpp
libtest_mom_hierarchy_handler_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_mom_hierarchy_handler_SOURCES = test_mom_hierarchy_handler.c

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh
TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov_core *.lo
