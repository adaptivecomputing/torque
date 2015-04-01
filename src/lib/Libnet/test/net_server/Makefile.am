PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libnet_server.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_net_server

libnet_server_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/net_server.cpp ${PROG_ROOT}/get_hostaddr.cpp
libnet_server_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_net_server_SOURCES = test_net_server.cpp

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
