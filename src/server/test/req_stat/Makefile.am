PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ --coverage `xml2-config --cflags`
AM_LIBS = `xml2-config --libs`

lib_LTLIBRARIES = libreq_stat.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_req_stat

libreq_stat_la_SOURCES = scaffolding.c ${PROG_ROOT}/req_stat.c
libreq_stat_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_req_stat_LDADD = ../../../test/torque_test_lib/libtorque_test.la ../../../test/scaffold_fail/libscaffold_fail.la
test_req_stat_SOURCES = test_req_stat.c

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh

TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
