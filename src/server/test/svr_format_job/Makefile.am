PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ --coverage

lib_LTLIBRARIES = libsvr_format_job.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_svr_format_job

libsvr_format_job_la_SOURCES = scaffolding.c ${PROG_ROOT}/svr_format_job.c
libsvr_format_job_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_svr_format_job_SOURCES = test_svr_format_job.c

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh

TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
