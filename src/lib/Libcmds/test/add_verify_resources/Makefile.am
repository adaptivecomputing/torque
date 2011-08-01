PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libadd_verify_resources.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_add_verify_resources

libadd_verify_resources_la_SOURCES = scaffolding.c ${PROG_ROOT}/add_verify_resources.c
libadd_verify_resources_la_LDFLAGS = @CHECK_LIBS@

test_add_verify_resources_SOURCES = test_add_verify_resources.c

check_SCRIPTS = coverage_run.sh

TESTS = ${check_PROGRAMS} coverage_run.sh

coverage_run.sh:
	echo 'cp -p .libs/add_verify_resources.gc* . >/dev/null 2>&1' > $@
	echo 'RESULTS=($$(gcov add_verify_resources.gcda))' >> $@
	echo 'PARSED_RESULT="TOTALCOV -- $${RESULTS[1]}: Lines($${RESULTS[5]})- $${RESULTS[3]}"' >> $@
	echo 'echo -e "\033[40m\033[1;33m$$PARSED_RESULT\033[0m"' >> $@
	chmod +x $@

CLEANFILES = coverage_run.sh *.gcno *.gcda *.gcov core *.lo
