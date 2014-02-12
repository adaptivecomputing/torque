PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ --coverage

lib_LTLIBRARIES = libreq_delete.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_req_delete

libreq_delete_la_SOURCES = scaffolding.c ${PROG_ROOT}/req_delete.c \
	${PROG_ROOT}/../lib/Libnet/get_hostname.c \
	${PROG_ROOT}/../lib/Libnet/net_cache.c \
	${PROG_ROOT}/../lib/Libnet/net_common.c \
	${PROG_ROOT}/../lib/Libutils/u_mu.c
	
libreq_delete_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_req_delete_LDADD = ../../../test/torque_test_lib/libtorque_test.la ../../../test/scaffold_fail/libscaffold_fail.la
test_req_delete_SOURCES = test_req_delete.c

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh

TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
