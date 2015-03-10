PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ --coverage

lib_LTLIBRARIES = libreq_deletearray.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_req_deletearray

libreq_deletearray_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/req_deletearray.cpp
libreq_deletearray_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_req_deletearray_LDADD = ../../../test/torque_test_lib/libtorque_test.la ../../../test/scaffold_fail/libscaffold_fail.la
test_req_deletearray_SOURCES = test_req_deletearray.cpp

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh

TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
