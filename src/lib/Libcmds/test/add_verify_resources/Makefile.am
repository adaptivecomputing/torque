PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libadd_verify_resources.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_add_verify_resources

libadd_verify_resources_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/add_verify_resources.cpp
libadd_verify_resources_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_add_verify_resources_SOURCES = test_add_verify_resources.cpp

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
