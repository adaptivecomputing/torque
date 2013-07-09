 
PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} -I$(PROG_ROOT)/../include --coverage `xml2-config --cflags`
AM_LIBS=`xml2-config --libs`

lib_LTLIBRARIES = libtest_execution_slot_tracker.la

AM_LDFLAGS = @CHECK_LIBS@ $(lib_LTLIBRARIES) $(AM_LIBS)

check_PROGRAMS = test_execution_slot_tracker

libtest_execution_slot_tracker_la_SOURCES = scaffolding.c $(PROG_ROOT)/execution_slot_tracker.cpp
libtest_execution_slot_tracker_la_LDFLAGS = @CHECK_LIBS@ $(AM_LIBS) -shared

test_execution_slot_tracker_SOURCES = test_execution_slot_tracker.c

check_SCRIPTS = coverage_run.sh

TESTS = $(check_PROGRAMS) coverage_run.sh

coverage_run.sh:
	echo 'cp -p .libs/execution_slot_tracker.gc* . >/dev/null 2>&1' > $@
	echo 'RESULTS=($$(gcov execution_slot_tracker.gcda))' >> $@
	echo 'PARSED_RESULT="TOTALCOV -- $${RESULTS[1]}: Lines($${RESULTS[5]})- $${RESULTS[3]}"' >> $@
	echo 'echo -e "\033[40m\033[1;33m$$PARSED_RESULT\033[0m"' >> $@
	chmod +x $@

CLEANFILES = coverage_run.sh *.gcno *.gcda *.gcov_core *.lo
