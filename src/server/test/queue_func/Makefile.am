PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ --coverage

lib_LTLIBRARIES = libqueue_func.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_queue_func

libqueue_func_la_SOURCES = scaffolding.c ${PROG_ROOT}/queue_func.c
libqueue_func_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_queue_func_LDADD = ../../../test/torque_test_lib/libtorque_test.la ../../../test/scaffold_fail/libscaffold_fail.la
test_queue_func_SOURCES = test_queue_func.c

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh

TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
