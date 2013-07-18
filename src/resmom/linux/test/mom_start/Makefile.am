PROG_ROOT = ../..

AM_CFLAGS = -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage -DPBS_MOM -DDEMUX=\"$(DEMUX_PATH)\"

lib_LTLIBRARIES = libmom_start.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_mom_start

libmom_start_la_SOURCES = scaffolding.c ${PROG_ROOT}/mom_start.c
libmom_start_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_mom_start_SOURCES = test_mom_start.c

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
