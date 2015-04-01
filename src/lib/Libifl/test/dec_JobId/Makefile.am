PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libdec_JobId.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_dec_JobId

libdec_JobId_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/dec_JobId.cpp
libdec_JobId_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_dec_JobId_SOURCES = test_dec_JobId.cpp

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS} 

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
