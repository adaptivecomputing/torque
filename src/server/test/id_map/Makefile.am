PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/../include --coverage

lib_LTLIBRARIES = libtest_id_map.la

AM_LDFLAGS = @CHECK_LIBS@ $(lib_LTLIBRARIES)

check_PROGRAMS = test_id_map

libtest_id_map_la_SOURCES = scaffolding.cpp $(PROG_ROOT)/id_map.cpp
libtest_id_map_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_id_map_SOURCES = test_id_map.cpp

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh
TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov_core *.lo
