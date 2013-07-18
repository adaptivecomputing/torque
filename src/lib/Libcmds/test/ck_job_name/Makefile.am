PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libck_job_name.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_ck_job_name

libck_job_name_la_SOURCES = scaffolding.c ${PROG_ROOT}/ck_job_name.c
libck_job_name_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_ck_job_name_SOURCES = test_ck_job_name.c

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
