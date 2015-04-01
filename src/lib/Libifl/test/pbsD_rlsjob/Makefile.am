PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libpbsD_rlsjob.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_pbsD_rlsjob

libpbsD_rlsjob_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/pbsD_rlsjob.cpp
libpbsD_rlsjob_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_pbsD_rlsjob_SOURCES = test_pbsD_rlsjob.cpp

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS} 

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
