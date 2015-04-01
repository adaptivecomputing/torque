PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ --coverage

lib_LTLIBRARIES = libarray_upgrade.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_array_upgrade

libarray_upgrade_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/array_upgrade.cpp
libarray_upgrade_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_array_upgrade_SOURCES = test_array_upgrade.cpp

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh

TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
