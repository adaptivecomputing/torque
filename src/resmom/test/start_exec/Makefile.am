PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage -I$(top_srcdir)/src/resmom/@PBS_MACH@ -DPBS_MOM -DDEMUX=\"$(program_prefix)$(DEMUX_PATH)$(program_suffix)\" -DRCP_PATH=\"$(program_prefix)$(RCP_PATH)$(program_suffix)\" -DRCP_ARGS=\"$(RCP_ARGS)\" -DPBS_SERVER_HOME=\"$(PBS_SERVER_HOME)\" -DPBS_ENVIRON=\"$(PBS_ENVIRON)\" `xml2-config --cflags`

AM_LIBS = `xml2-config --libs`


lib_LTLIBRARIES = libuut.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_uut

libuut_la_SOURCES =  ${PROG_ROOT}/start_exec.cpp  scaffolding.c
libuut_la_LDFLAGS = @CHECK_LIBS@ -lgcov -shared
test_uut_LDADD = ../../../test/torque_test_lib/libtorque_test.la ../../../test/scaffold_fail/libscaffold_fail.la

test_uut_SOURCES = test_uut.cpp 

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh

TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
