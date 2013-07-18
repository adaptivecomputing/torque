PROG_ROOT = ../..

if USE_TCLQSTAT
TCLQSTATLIBS = $(MY_TCL_LIBS)
TCLQSTATCFLAGS = $(MY_TCL_INCS)
endif

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage ${TCLQSTATCFLAGS}

lib_LTLIBRARIES = libqstat.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES} ${TCLQSTATLIBS}

check_PROGRAMS = test_qstat

libqstat_la_SOURCES = scaffolding.c ${PROG_ROOT}/qstat.c
libqstat_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_qstat_SOURCES = test_qstat.c

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh

TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
