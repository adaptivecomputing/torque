PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libpbsD_statnode.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_pbsD_statnode

libpbsD_statnode_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/pbsD_statnode.cpp
libpbsD_statnode_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_pbsD_statnode_SOURCES = test_pbsD_statnode.cpp

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS} 

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
