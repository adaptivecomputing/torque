
PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} -I$(PROG_ROOT)/../include --coverage

lib_LTLIBRARIES = libtest_login_nodes.la

AM_LDFLAGS = @CHECK_LIBS@ $(lib_LTLIBRARIES)

check_PROGRAMS = test_login_nodes

libtest_login_nodes_la_SOURCES = scaffolding.c $(PROG_ROOT)/login_nodes.c
libtest_login_nodes_la_LDFLAGS = @CHECK_LIBS@ $(AM_LIBS) -shared

test_login_nodes_SOURCES = test_login_nodes.c

check_SCRIPTS = coverage_run.sh

TESTS = $(check_PROGRAMS) coverage_run.sh

coverage_run.sh:
	echo 'cp -p .libs/login_nodes.gc* . >/dev/null 2>&1' > $@
	echo 'RESULTS=($$(gcov login_nodes.gcda))' >> $@
	echo 'PARSED_RESULT="TOTALCOV -- $${RESULTS[1]}: Lines($${RESULTS[5]})- $${RESULTS[3]}"' >> $@
	echo 'echo -e "\033[40m\033[1;33m$$PARSED_RESULT\033[0m"' >> $@
	chmod +x $@

CLEANFILES = coverage_run.sh *.gcno *.gcda *.gcov_core *.lo

