PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libget_svrport.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_get_svrport

libget_svrport_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/get_svrport.cpp
libget_svrport_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_get_svrport_SOURCES = test_get_svrport.cpp

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS} 

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
