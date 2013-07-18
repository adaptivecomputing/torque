PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ --coverage

lib_LTLIBRARIES = libjob_qs_upgrade.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_job_qs_upgrade

libjob_qs_upgrade_la_SOURCES = scaffolding.c ${PROG_ROOT}/job_qs_upgrade.c
libjob_qs_upgrade_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_job_qs_upgrade_SOURCES = test_job_qs_upgrade.c

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh

TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
