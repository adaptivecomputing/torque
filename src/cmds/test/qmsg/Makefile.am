PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libqmsg.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_qmsg

libqmsg_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/qmsg.cpp
libqmsg_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_qmsg_SOURCES = test_qmsg.cpp

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh

TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
