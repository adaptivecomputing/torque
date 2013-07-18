PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ --coverage

lib_LTLIBRARIES = libdis_read.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_dis_read

libdis_read_la_SOURCES = scaffolding.c ${PROG_ROOT}/dis_read.c
libdis_read_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_dis_read_SOURCES = test_dis_read.c

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh

TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
