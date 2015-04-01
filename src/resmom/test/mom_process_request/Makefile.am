PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage -I$(top_srcdir)/src/resmom/@PBS_MACH@ -DPBS_MOM -DDEMUX=\"$(program_prefix)$(DEMUX_PATH)$(program_suffix)\" -DRCP_PATH=\"$(program_prefix)$(RCP_PATH)$(program_suffix)\" -DRCP_ARGS=\"$(RCP_ARGS)\" -DPBS_SERVER_HOME=\"$(PBS_SERVER_HOME)\" -DPBS_ENVIRON=\"$(PBS_ENVIRON)\"

lib_LTLIBRARIES = libmom_process_request.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_mom_process_request

libmom_process_request_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/mom_process_request.cpp ${PROG_ROOT}/../lib/Libnet/get_hostaddr.c
libmom_process_request_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_mom_process_request_SOURCES = test_mom_process_request.c

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh

TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
