PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libjob_recycler.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_job_recycler

libjob_recycler_la_SOURCES = scaffolding.c ${PROG_ROOT}/job_recycler.c ${PROG_ROOT}/../lib/Libutils/u_resizable_array.c  ${PROG_ROOT}/../lib/Libutils/u_hash_table.c
libjob_recycler_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_job_recycler_SOURCES = test_job_recycler.c

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh

TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
