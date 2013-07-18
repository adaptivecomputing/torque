PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libcvtdate.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_cvtdate

libcvtdate_la_SOURCES = scaffolding.c ${PROG_ROOT}/cvtdate.c
libcvtdate_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_cvtdate_SOURCES = test_cvtdate.c

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
