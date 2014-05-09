 
PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} -I$(PROG_ROOT)/../include --coverage `xml2-config --cflags`

AM_LIBS=`xml2-config --libs`

lib_LTLIBRARIES = libtest_id_map.la

AM_LDFLAGS = @CHECK_LIBS@ $(lib_LTLIBRARIES) $(AM_LIBS)

check_PROGRAMS = test_id_map

libtest_id_map_la_SOURCES = scaffolding.c $(PROG_ROOT)/id_map.cpp
libtest_id_map_la_LDFLAGS = @CHECK_LIBS@ $(AM_LIBS) -shared

test_id_map_SOURCES = test_id_map.c

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh
TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov_core *.lo
