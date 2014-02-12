PROG_ROOT = ../..

AM_CFLAGS = -DPENABLE_LINUX26_CPUSETS -include pbs_config.h -include pbs_nodes.h -include cpuset_macros.h -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libcpuset.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_cpuset

libcpuset_la_SOURCES = scaffolding.c ${PROG_ROOT}/numa_node.cpp ${PROG_ROOT}/node_internals.cpp ${PROG_ROOT}/cpuset.c
libcpuset_la_LDFLAGS = @CHECK_LIBS@ -shared -lhwloc -lgcov

test_cpuset_SOURCES = test_cpuset.c

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = coverage_run.sh *.gcno *.gcda *.gcov core *.lo
