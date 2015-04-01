PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libsetup_env.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_setup_env

libsetup_env_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/setup_env.cpp
libsetup_env_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_setup_env_SOURCES = test_setup_env.cpp

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
