PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libu_mu.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_u_mu

libu_mu_la_SOURCES = scaffolding.c ${PROG_ROOT}/u_mu.c
libu_mu_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_u_mu_SOURCES = test_u_mu.c

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
