PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libget_hostname.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_get_hostname

libget_hostname_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/get_hostname.cpp
libget_hostname_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_get_hostname_SOURCES = test_get_hostname.cpp

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
