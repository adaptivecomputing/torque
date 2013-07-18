PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ --coverage

lib_LTLIBRARIES = libreply_send.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_reply_send

libreply_send_la_SOURCES = scaffolding.c ${PROG_ROOT}/reply_send.c
libreply_send_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_reply_send_SOURCES = test_reply_send.c

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh

TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
