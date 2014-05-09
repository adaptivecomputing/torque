PROG_ROOT = ../..
  
AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} -I$(PROG_ROOT)/../include --coverage `xml2-config --cflags`
AM_LIBS=`xml2-config --libs`
  
lib_LTLIBRARIES = libtest_incoming_request.la

AM_LDFLAGS = @CHECK_LIBS@ $(lib_LTLIBRARIES) $(AM_LIBS)

check_PROGRAMS = test_incoming_request

libtest_incoming_request_la_SOURCES = scaffolding.c $(PROG_ROOT)/incoming_request.c
libtest_incoming_request_la_LDFLAGS = @CHECK_LIBS@ $(AM_LIBS) -shared -lgcov

test_incoming_request_SOURCES = test_incoming_request.c

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh
TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov_core *.lo
