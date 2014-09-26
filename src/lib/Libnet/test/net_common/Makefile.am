PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libnet_common.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_net_common

libnet_common_la_SOURCES = scaffolding.c ${PROG_ROOT}/net_common.c ${PROG_ROOT}/../Libutils/u_mutex_mgr.cpp
libnet_common_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_net_common_SOURCES = test_net_common.c

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
