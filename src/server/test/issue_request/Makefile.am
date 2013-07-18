PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ --coverage

lib_LTLIBRARIES = libissue_request.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_issue_request

libissue_request_la_SOURCES = scaffolding.c ${PROG_ROOT}/issue_request.c
libissue_request_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_issue_request_SOURCES = test_issue_request.c

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh

TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
