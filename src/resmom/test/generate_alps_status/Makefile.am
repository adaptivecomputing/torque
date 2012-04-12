
PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION `xml2-config --cflags` -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} -I$(PROG_ROOT)/../include --coverage -I$(top_srcdir)/src/resmom/@PBS_MACH@ -DPBS_MOM -DDEMUX=\"$(program_prefix)$(DEMUX_PATH)$(program_suffix)\" -DRCP_PATH=\"$(program_prefix)$(RCP_PATH)$(program_suffix)\" -DRCP_ARGS=\"$(RCP_ARGS)\" -DPBS_SERVER_HOME=\"$(PBS_SERVER_HOME)\" -DPBS_ENVIRON=\"$(PBS_ENVIRON)\"

AM_LIBS = `xml2-config --libs`

lib_LTLIBRARIES = libtest_generate_alps_status.la

AM_LDFLAGS = @CHECK_LIBS@ $(lib_LTLIBRARIES) $(AM_LIBS)

check_PROGRAMS = test_generate_alps_status

libtest_generate_alps_status_la_SOURCES = scaffolding.c $(PROG_ROOT)/generate_alps_status.c $(PROG_ROOT)/../lib/Libutils/u_constants.c
libtest_generate_alps_status_la_LDFLAGS = @CHECK_LIBS@ $(AM_LIBS) -shared

test_generate_alps_status_SOURCES = test_generate_alps_status.c

check_SCRIPTS = coverage_run.sh

TESTS = $(check_PROGRAMS) coverage_run.sh

coverage_run.sh:
	echo 'cp -p .libs/generate_alps_status.gc* . >/dev/null 2>&1' > $@
	echo 'RESULTS=($$(gcov generate_alps_status.gcda))' >> $@
	echo 'PARSED_RESULT="TOTALCOV -- $${RESULTS[1]}: Lines($${RESULTS[5]})- $${RESULTS[3]}"' >> $@
	echo 'echo -e "\033[40m\033[1;33m$$PARSED_RESULT\033[0m"' >> $@
	chmod +x $@

CLEANFILES = coverage_run.sh *.gcno *.gcda *.gcov_core *.lo

