PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libPBSD_submit_caps.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_PBSD_submit_caps

libPBSD_submit_caps_la_SOURCES = scaffolding.c ${PROG_ROOT}/PBSD_submit_caps.c
libPBSD_submit_caps_la_LDFLAGS = @CHECK_LIBS@

test_PBSD_submit_caps_SOURCES = test_PBSD_submit_caps.c

check_SCRIPTS = coverage_run.sh

TESTS = ${check_PROGRAMS} coverage_run.sh

coverage_run.sh:
	echo 'cp -p .libs/PBSD_submit_caps.gc* . >/dev/null 2>&1' > $@
	echo 'RESULTS=($$(gcov PBSD_submit_caps.gcda))' >> $@
	echo 'PARSED_RESULT="TOTALCOV -- $${RESULTS[1]}: Lines($${RESULTS[5]})- $${RESULTS[3]}"' >> $@
	echo 'echo -e "\033[40m\033[1;33m$$PARSED_RESULT\033[0m"' >> $@
	chmod +x $@

CLEANFILES = coverage_run.sh *.gcno *.gcda *.gcov core *.lo
