PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libsite_map_usr.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_site_map_usr

libsite_map_usr_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/site_map_usr.cpp
libsite_map_usr_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_site_map_usr_SOURCES = test_site_map_usr.cpp

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
