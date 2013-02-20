PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ --coverage

lib_LTLIBRARIES = libjob_func.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_record_jobinfo test_job_func

libjob_func_la_SOURCES = ${PROG_ROOT}/job_attr_def.c scaffolding.c ${PROG_ROOT}/job_func.c scaffolding_job_attr_def.c
libjob_func_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_job_func_LDADD = ../../../test/torque_test_lib/libtorque_test.la ../../../test/scaffold_fail/libscaffold_fail.la
test_job_func_SOURCES = test_job_func.c
test_record_jobinfo_LDADD = ../../../test/torque_test_lib/libtorque_test.la ../../../test/scaffold_fail/libscaffold_fail.la
test_record_jobinfo_SOURCES = test_record_jobinfo.c

check_SCRIPTS = coverage_run.sh

TESTS = ${check_PROGRAMS} coverage_run.sh

coverage_run.sh:
	echo 'cp -p .libs/job_func.gc* . >/dev/null 2>&1' > $@
	echo 'RESULTS=($$(gcov job_func.gcda))' >> $@
	echo 'PARSED_RESULT="TOTALCOV -- $${RESULTS[1]}: Lines($${RESULTS[5]})- $${RESULTS[3]}"' >> $@
	echo 'echo -e "\033[40m\033[1;33m$$PARSED_RESULT\033[0m"' >> $@
	chmod +x $@

CLEANFILES = coverage_run.sh *.gcno *.gcda *.gcov core *.lo
