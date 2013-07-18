PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libMXML.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_MXML

libMXML_la_SOURCES = scaffolding.c ${PROG_ROOT}/MXML.c
libMXML_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_MXML_SOURCES = test_MXML.c

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
