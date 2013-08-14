PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libnet_cache.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_net_cache

libnet_cache_la_SOURCES = scaffolding.c ${PROG_ROOT}/net_cache.c ${PROG_ROOT}/../Libutils/u_hash_table.c
libnet_cache_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_net_cache_SOURCES = test_net_cache.c

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
