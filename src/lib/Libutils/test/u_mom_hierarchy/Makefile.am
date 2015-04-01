PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libu_mom_hierarchy.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_u_mom_hierarchy

libu_mom_hierarchy_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/u_mom_hierarchy.cpp
libu_mom_hierarchy_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_u_mom_hierarchy_SOURCES = test_u_mom_hierarchy.cpp

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
