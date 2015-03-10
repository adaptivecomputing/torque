PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libuut.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_uut

libuut_la_SOURCES =  ${PROG_ROOT}/attr_fn_resc.cpp  scaffolding.cpp  ${PROG_ROOT}/../Libcsv/csv.cpp
libuut_la_LDFLAGS = @CHECK_LIBS@ -lgcov
test_uut_LDADD = ../../../../test/torque_test_lib/libtorque_test.la ../../../../test/scaffold_fail/libscaffold_fail.la

test_uut_SOURCES = test_uut.cpp 

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
