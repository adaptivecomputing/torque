PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libtcp_dis.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_tcp_dis

libtcp_dis_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/tcp_dis.cpp
libtcp_dis_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_tcp_dis_SOURCES = test_tcp_dis.cpp

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS} 

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
