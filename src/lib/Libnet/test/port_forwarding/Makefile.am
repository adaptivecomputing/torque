PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libport_forwarding.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_port_forwarding

libport_forwarding_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/port_forwarding.cpp
libport_forwarding_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_port_forwarding_SOURCES = test_port_forwarding.cpp

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
