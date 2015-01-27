PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage -DPBS_MOM -DDEMUX=\"$(DEMUX_PATH)\"

lib_LTLIBRARIES = libmom_mach.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_mom_mach

libmom_mach_la_SOURCES = scaffolding.c ${PROG_ROOT}/mom_mach.c
libmom_mach_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_mom_mach_SOURCES = test_mom_mach.c
 
check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
