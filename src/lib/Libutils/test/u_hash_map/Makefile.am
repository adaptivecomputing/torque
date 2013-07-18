 
PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} -I$(PROG_ROOT)/../include --coverage `xml2-config --cflags`
AM_LIBS=`xml2-config --libs`

lib_LTLIBRARIES = libtest_u_hash_map.la

AM_LDFLAGS = @CHECK_LIBS@ $(lib_LTLIBRARIES) $(AM_LIBS)

check_PROGRAMS = test_u_hash_map

libtest_u_hash_map_la_SOURCES = scaffolding.c $(PROG_ROOT)/u_hash_map.c
libtest_u_hash_map_la_LDFLAGS = @CHECK_LIBS@ $(AM_LIBS) -shared -lgcov

test_u_hash_map_SOURCES = test_u_hash_map.c

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
