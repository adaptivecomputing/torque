 
PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} -I$(PROG_ROOT)/../include --coverage 
AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} -I$(PROG_ROOT)/../include --coverage

lib_LTLIBRARIES = libtest_req.la

AM_LDFLAGS = @CHECK_LIBS@ $(lib_LTLIBRARIES)

check_PROGRAMS = test_req

libtest_req_la_SOURCES = scaffolding.c $(PROG_ROOT)/req.cpp $(PROG_ROOT)/../Libutils/allocation.cpp \
												 $(PROG_ROOT)/../Libutils/u_misc.c
libtest_req_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_req_SOURCES = test_req.c

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov_core *.lo
