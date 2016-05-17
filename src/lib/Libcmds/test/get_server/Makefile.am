include $(top_srcdir)/buildutils/config.mk

PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage -DPBS_DEFAULT_FILE=\"$(PBS_DEFAULT_FILE)\"


lib_LTLIBRARIES = libget_server.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_get_server

libget_server_la_SOURCES = scaffolding.c ${PROG_ROOT}/get_server.c ${PROG_ROOT}/parse_jobid.c
libget_server_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_get_server_SOURCES = test_get_server.c

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
