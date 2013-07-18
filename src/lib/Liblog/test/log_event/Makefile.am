PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = liblog_event.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_log_event

liblog_event_la_SOURCES = scaffolding.c ${PROG_ROOT}/log_event.c
liblog_event_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_log_event_SOURCES = test_log_event.c

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
