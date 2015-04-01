PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ --coverage

lib_LTLIBRARIES = libsvr_movejob.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_svr_movejob

libsvr_movejob_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/svr_movejob.cpp
libsvr_movejob_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_svr_movejob_LDADD = ../../../test/torque_test_lib/libtorque_test.la ../../../test/scaffold_fail/libscaffold_fail.la
test_svr_movejob_SOURCES = test_svr_movejob.cpp

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh

TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
