PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} -I$(PROG_ROOT)/../include --coverage

lib_LTLIBRARIES = libtest_batch_request.la

AM_LDFLAGS = @CHECK_LIBS@ $(lib_LTLIBRARIES) $(AM_LIBS)

check_PROGRAMS = test_batch_request

libtest_batch_request_la_SOURCES = scaffolding.c $(PROG_ROOT)/batch_request.c 
libtest_batch_request_la_LDFLAGS = @CHECK_LIBS@ $(AM_LIBS) -shared

test_batch_request_SOURCES = test_batch_request.c

check_SCRIPTS = coverage_run.sh

TESTS = $(check_PROGRAMS) coverage_run.sh

coverage_run.sh:
	echo 'cp -p .libs/batch_request.gc* . >/dev/null 2>&1' > $@
	echo 'RESULTS=($$(gcov batch_request.gcda))' >> $@
	echo 'PARSED_RESULT="TOTALCOV -- $${RESULTS[1]}: Lines($${RESULTS[5]})- $${RESULTS[3]}"' >> $@
	echo 'echo -e "\033[40m\033[1;33m$$PARSED_RESULT\033[0m"' >> $@
	chmod +x $@
  
CLEANFILES = coverage_run.sh *.gcno *.gcda *.gcov_core *.lo
