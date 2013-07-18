PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libsite_map_usr.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_site_map_usr

libsite_map_usr_la_SOURCES = scaffolding.c ${PROG_ROOT}/site_map_usr.c
libsite_map_usr_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_site_map_usr_SOURCES = test_site_map_usr.c

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
