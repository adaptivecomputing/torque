PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ --coverage

lib_LTLIBRARIES = libgeteusernam.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_geteusernam

libgeteusernam_la_SOURCES = scaffolding.c ${PROG_ROOT}/geteusernam.c
libgeteusernam_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_geteusernam_SOURCES = test_geteusernam.c

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh

TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
