PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ --coverage

lib_LTLIBRARIES = libsvr_jobfunc.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_svr_jobfunc

libsvr_jobfunc_la_SOURCES =  ${PROG_ROOT}/svr_jobfunc.cpp ${PROG_ROOT}/resc_def_all.cpp scaffolding.cpp
libsvr_jobfunc_la_LDFLAGS = @CHECK_LIBS@ -lgcov -shared

test_svr_jobfunc_LDADD = ../../../test/torque_test_lib/libtorque_test.la ../../../test/scaffold_fail/libscaffold_fail.la
test_svr_jobfunc_SOURCES = test_svr_jobfunc.cpp

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh

TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
