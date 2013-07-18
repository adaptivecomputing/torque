PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ --coverage

lib_LTLIBRARIES = libresc_def_all.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_resc_def_all

libresc_def_all_la_SOURCES = scaffolding.c ${PROG_ROOT}/resc_def_all.c
libresc_def_all_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_resc_def_all_SOURCES = test_resc_def_all.c

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh

TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
