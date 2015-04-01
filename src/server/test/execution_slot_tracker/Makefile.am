PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I$(PROG_ROOT)/../include --coverage

lib_LTLIBRARIES = libtest_execution_slot_tracker.la

AM_LDFLAGS = @CHECK_LIBS@ $(lib_LTLIBRARIES)

check_PROGRAMS = test_execution_slot_tracker

libtest_execution_slot_tracker_la_SOURCES = scaffolding.cpp $(PROG_ROOT)/execution_slot_tracker.cpp
libtest_execution_slot_tracker_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_execution_slot_tracker_SOURCES = test_execution_slot_tracker.cpp

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh

TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
