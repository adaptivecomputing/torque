PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage -I$(top_srcdir)/src/resmom/@PBS_MACH@ -DPBS_MOM -DDEMUX=\"$(program_prefix)$(DEMUX_PATH)$(program_suffix)\" -DRCP_PATH=\"$(program_prefix)$(RCP_PATH)$(program_suffix)\" -DRCP_ARGS=\"$(RCP_ARGS)\" -DPBS_SERVER_HOME=\"$(PBS_SERVER_HOME)\" -DPBS_ENVIRON=\"$(PBS_ENVIRON)\"

lib_LTLIBRARIES = libstart_exec.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_start_exec

libstart_exec_la_SOURCES = scaffolding.c ${PROG_ROOT}/start_exec.c
libstart_exec_la_LDFLAGS = @CHECK_LIBS@

test_start_exec_SOURCES = test_start_exec.c

check_SCRIPTS = coverage_run.sh

TESTS = ${check_PROGRAMS} coverage_run.sh

coverage_run.sh:
	echo 'cp -p .libs/start_exec.gc* . >/dev/null 2>&1' > $@
	echo 'RESULTS=($$(gcov start_exec.gcda))' >> $@
	echo 'PARSED_RESULT="TOTALCOV -- $${RESULTS[1]}: Lines($${RESULTS[5]})- $${RESULTS[3]}"' >> $@
	echo 'echo -e "\033[40m\033[1;33m$$PARSED_RESULT\033[0m"' >> $@
	chmod +x $@

CLEANFILES = coverage_run.sh *.gcno *.gcda *.gcov core *.lo
