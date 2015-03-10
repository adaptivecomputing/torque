PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ --coverage

lib_LTLIBRARIES = libjob_func.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_record_jobinfo test_job_func

libjob_func_la_SOURCES = ${PROG_ROOT}/job_attr_def.cpp scaffolding.cpp ${PROG_ROOT}/job_func.cpp scaffolding_job_attr_def.cpp
libjob_func_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_job_func_LDADD = ../../../test/torque_test_lib/libtorque_test.la ../../../test/scaffold_fail/libscaffold_fail.la
test_job_func_SOURCES = test_job_func.cpp
test_record_jobinfo_LDADD = ../../../test/torque_test_lib/libtorque_test.la ../../../test/scaffold_fail/libscaffold_fail.la
test_record_jobinfo_SOURCES = test_record_jobinfo.cpp

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh

TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
