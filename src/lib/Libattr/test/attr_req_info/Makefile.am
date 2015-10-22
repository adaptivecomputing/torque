 
PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} -I$(PROG_ROOT)/../include --coverage
AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} -I$(PROG_ROOT)/../include --coverage

lib_LTLIBRARIES = libtest_attr_req_info.la

AM_LDFLAGS = @CHECK_LIBS@ $(lib_LTLIBRARIES)

check_PROGRAMS = test_attr_req_info

libtest_attr_req_info_la_SOURCES = scaffolding.c $(PROG_ROOT)/attr_req_info.cpp 
libtest_attr_req_info_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_attr_req_info = test_attr_req_info.c

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh
TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov_core *.lo
