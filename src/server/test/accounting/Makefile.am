PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I$(PROG_ROOT)/ -I$(PROG_ROOT)/../include/ --coverage 

lib_LTLIBRARIES = libaccounting.la

AM_LDFLAGS = @CHECK_LIBS@ $(lib_LTLIBRARIES)

check_PROGRAMS = test_accounting

libaccounting_la_SOURCES = scaffolding.c $(PROG_ROOT)/accounting.c
libaccounting_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov  

test_accounting_SOURCES = test_accounting.c

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh

TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
