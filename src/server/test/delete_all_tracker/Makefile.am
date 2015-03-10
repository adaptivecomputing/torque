PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/../include --coverage 

lib_LTLIBRARIES = libtest_delete_all_tracker.la

AM_LDFLAGS = @CHECK_LIBS@ $(lib_LTLIBRARIES)

check_PROGRAMS = test_delete_all_tracker

libtest_delete_all_tracker_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/delete_all_tracker.cpp
libtest_delete_all_tracker_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_delete_all_tracker_SOURCES = test_delete_all_tracker.cpp

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh
TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov_core *.lo
