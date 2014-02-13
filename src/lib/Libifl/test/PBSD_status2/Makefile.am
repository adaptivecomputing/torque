PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libPBSD_status2.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_PBSD_status2

libPBSD_status2_la_SOURCES = scaffolding.c ${PROG_ROOT}/PBSD_status2.c
libPBSD_status2_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov
test_PBSD_status2_LDADD = ../../../../test/torque_test_lib/libtorque_test.la ../../../../test/scaffold_fail/libscaffold_fail.la

test_PBSD_status2_SOURCES = test_PBSD_status2.c

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS} 

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
