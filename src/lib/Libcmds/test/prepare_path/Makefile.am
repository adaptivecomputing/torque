PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libprepare_path.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_prepare_path

libprepare_path_la_SOURCES = scaffolding.c ${PROG_ROOT}/prepare_path.c
libprepare_path_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_prepare_path_SOURCES = test_prepare_path.c

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
