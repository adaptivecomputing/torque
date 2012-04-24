
PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} -I$(PROG_ROOT)/../include --coverage `xml2-config --cflags` 
AM_LIBS = `xml2-config --libs`

lib_LTLIBRARIES = libtest_track_alps_reservations.la

AM_LDFLAGS = @CHECK_LIBS@ $(lib_LTLIBRARIES) $(AM_LIBS)

check_PROGRAMS = test_track_alps_reservations

libtest_track_alps_reservations_la_SOURCES = scaffolding.c $(PROG_ROOT)/track_alps_reservations.c $(PROG_ROOT)/../lib/Libutils/u_constants.c
libtest_track_alps_reservations_la_LDFLAGS = @CHECK_LIBS@ $(AM_LIBS) -shared

test_track_alps_reservations_SOURCES = test_track_alps_reservations.c

check_SCRIPTS = coverage_run.sh

TESTS = $(check_PROGRAMS) coverage_run.sh

coverage_run.sh:
	echo 'cp -p .libs/track_alps_reservations.gc* . >/dev/null 2>&1' > $@
	echo 'RESULTS=($$(gcov track_alps_reservations.gcda))' >> $@
	echo 'PARSED_RESULT="TOTALCOV -- $${RESULTS[1]}: Lines($${RESULTS[5]})- $${RESULTS[3]}"' >> $@
	echo 'echo -e "\033[40m\033[1;33m$$PARSED_RESULT\033[0m"' >> $@
	chmod +x $@

CLEANFILES = coverage_run.sh *.gcno *.gcda *.gcov_core *.lo

