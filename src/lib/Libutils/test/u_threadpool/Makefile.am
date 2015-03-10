PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libu_threadpool.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_u_threadpool

libu_threadpool_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/u_threadpool.cpp
libu_threadpool_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_u_threadpool_SOURCES = test_u_threadpool.cpp

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
