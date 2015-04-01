PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libnonblock.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_nonblock

libnonblock_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/nonblock.cpp
libnonblock_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_nonblock_SOURCES = test_nonblock.cpp

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS} 

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
