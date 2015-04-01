PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libcnt2server.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_cnt2server

libcnt2server_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/cnt2server.cpp ${PROG_ROOT}/../Libcsv/csv.cpp
libcnt2server_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_cnt2server_SOURCES = test_cnt2server.cpp

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
