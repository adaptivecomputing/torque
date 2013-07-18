PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libpbs_messages.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_pbs_messages

libpbs_messages_la_SOURCES = scaffolding.c ${PROG_ROOT}/pbs_messages.c
libpbs_messages_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_pbs_messages_SOURCES = test_pbs_messages.c

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
