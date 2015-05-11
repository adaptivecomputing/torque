PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage -DPBS_MOM -DPENABLE_LINUX26_CPUSETS
AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage -DPBS_MOM -DPENABLE_LINUX26_CPUSETS

lib_LTLIBRARIES = libnuma_node.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_numa_node

libnuma_node_la_SOURCES = scaffolding.c ${PROG_ROOT}/numa_node.cpp ${PROG_ROOT}/../../lib/Libutils/u_misc.c ${PROG_ROOT}/../../lib/Libutils/allocation.cpp
libnuma_node_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_numa_node_SOURCES = test_numa_node.c

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = coverage_run.sh *.gcno *.gcda *.gcov core *.lo
