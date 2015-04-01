 
PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} -I$(PROG_ROOT)/../include --coverage `xml2-config --cflags`
AM_LIBS=`xml2-config --libs`

lib_LTLIBRARIES = libtest_get_path_jobdata.la

AM_LDFLAGS = @CHECK_LIBS@ $(lib_LTLIBRARIES) $(AM_LIBS)

check_PROGRAMS = test_get_path_jobdata

libtest_get_path_jobdata_la_SOURCES = scaffolding.cpp $(PROG_ROOT)/get_path_jobdata.cpp
libtest_get_path_jobdata_la_LDFLAGS = @CHECK_LIBS@ $(AM_LIBS) -shared -lgcov

test_get_path_jobdata_SOURCES = test_get_path_jobdata.cpp

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh
TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov_core *.lo
