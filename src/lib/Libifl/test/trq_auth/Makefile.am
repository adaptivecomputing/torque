PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libtrq_auth.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_trq_auth

libtrq_auth_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/trq_auth.cpp
libtrq_auth_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_trq_auth_SOURCES = test_trq_auth.cpp

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS} 

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
