PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libjob_qs_upgrade.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_job_qs_upgrade

libjob_qs_upgrade_la_SOURCES = scaffolding.c ${PROG_ROOT}/job_qs_upgrade.c
libjob_qs_upgrade_la_LDFLAGS = @CHECK_LIBS@

test_job_qs_upgrade_SOURCES = test_job_qs_upgrade.c

check_SCRIPTS = coverage_run.sh

TESTS = ${check_PROGRAMS} coverage_run.sh

coverage_run.sh:
	echo 'cp -p .libs/job_qs_upgrade.gc* . >/dev/null 2>&1' > $@
	echo 'RESULTS=($$(gcov job_qs_upgrade.gcda))' >> $@
	echo 'PARSED_RESULT="TOTALCOV -- $${RESULTS[1]}: Lines($${RESULTS[5]})- $${RESULTS[3]}"' >> $@
	echo 'echo -e "\033[40m\033[1;33m$$PARSED_RESULT\033[0m"' >> $@
	chmod +x $@

CLEANFILES = coverage_run.sh *.gcno *.gcda *.gcov core *.lo
