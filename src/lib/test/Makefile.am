PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libscaffolding_lib.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

libscaffolding_lib_la_SOURCES = scaffolding_lib.c
libscaffolding_lib_la_LDFLAGS = @CHECK_LIBS@ -shared

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
