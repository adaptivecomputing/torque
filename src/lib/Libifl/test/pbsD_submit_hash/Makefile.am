PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libpbsD_submit_hash.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_pbsD_submit_hash

libpbsD_submit_hash_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/pbsD_submit_hash.cpp
libpbsD_submit_hash_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_pbsD_submit_hash_SOURCES = test_pbsD_submit_hash.cpp

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS} 

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
