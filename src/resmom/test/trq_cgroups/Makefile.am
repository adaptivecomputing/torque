 
PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -DPBS_SERVER_HOME=\"$(PBS_SERVER_HOME)\" -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} -I$(PROG_ROOT)/../include --coverage `xml2-config --cflags`

lib_LTLIBRARIES = libtest_trq_cgroups.la

AM_LDFLAGS = @CHECK_LIBS@ $(lib_LTLIBRARIES) $(HWLOC_LIBS)

check_PROGRAMS = test_trq_cgroups

libtest_trq_cgroups_la_SOURCES = scaffolding.c $(PROG_ROOT)/trq_cgroups.c
libtest_trq_cgroups_la_LDFLAGS = @CHECK_LIBS@  -shared -lgcov

test_trq_cgroups_SOURCES = test_trq_cgroups.c

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh
TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov_core *.lo
