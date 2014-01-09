 
PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} -I$(PROG_ROOT)/../include --coverage `xml2-config --cflags` -DPBS_MOM
AM_LIBS=`xml2-config --libs`

lib_LTLIBRARIES = libtest_parse_config.la

AM_LDFLAGS = @CHECK_LIBS@ $(lib_LTLIBRARIES) $(AM_LIBS)

check_PROGRAMS = test_parse_config

libtest_parse_config_la_SOURCES = scaffolding.c $(PROG_ROOT)/parse_config.c
libtest_parse_config_la_LDFLAGS = @CHECK_LIBS@ $(AM_LIBS) -shared -lgcov

test_parse_config_SOURCES = test_parse_config.c

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh
TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov_core *.lo
