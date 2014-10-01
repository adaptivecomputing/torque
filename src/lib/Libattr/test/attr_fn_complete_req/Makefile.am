 
PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} -I$(PROG_ROOT)/../include --coverage

lib_LTLIBRARIES = libtest_attr_fn_complete_req.la

AM_LDFLAGS = @CHECK_LIBS@ $(lib_LTLIBRARIES) $(AM_LIBS)

check_PROGRAMS = test_attr_fn_complete_req

libtest_attr_fn_complete_req_la_SOURCES = scaffolding.c ${PROG_ROOT}/attr_fn_complete_req.c \
																					${PROG_ROOT}/req.cpp ${PROG_ROOT}/complete_req.cpp
libtest_attr_fn_complete_req_la_LDFLAGS = @CHECK_LIBS@ ${AM_LIBS} -shared -lgcov

test_attr_fn_complete_req_SOURCES = test_attr_fn_complete_req.c

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh
TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov_core *.lo
