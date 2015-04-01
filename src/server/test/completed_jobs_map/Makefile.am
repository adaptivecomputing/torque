PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} -I$(PROG_ROOT)/../include --coverage `xml2-config --cflags`
AM_LIBS=`xml2-config --libs`

lib_LTLIBRARIES = libtest_completed_jobs_map.la

AM_LDFLAGS = @CHECK_LIBS@ $(lib_LTLIBRARIES) $(AM_LIBS)

check_PROGRAMS = test_completed_jobs_map

libtest_completed_jobs_map_la_SOURCES = scaffolding.cpp $(PROG_ROOT)/completed_jobs_map.cpp
libtest_completed_jobs_map_la_LDFLAGS = @CHECK_LIBS@ $(AM_LIBS) -shared -lgcov

test_completed_jobs_map_SOURCES = test_completed_jobs_map.cpp

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh
TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov_core *.lo
