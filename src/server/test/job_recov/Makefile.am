PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ --coverage

lib_LTLIBRARIES = libjob_recov.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_job_recov

libjob_recov_la_SOURCES = scaffolding.c ${PROG_ROOT}/job_recov.c
libjob_recov_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_job_recov_SOURCES = test_job_recov.c

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh

TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
