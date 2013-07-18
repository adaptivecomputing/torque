PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libport_forwarding.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_port_forwarding

libport_forwarding_la_SOURCES = scaffolding.c ${PROG_ROOT}/port_forwarding.c
libport_forwarding_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_port_forwarding_SOURCES = test_port_forwarding.c

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
