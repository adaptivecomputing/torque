PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ --coverage

lib_LTLIBRARIES = libgeteusernam.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_geteusernam

libgeteusernam_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/geteusernam.cpp
libgeteusernam_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_geteusernam_SOURCES = test_geteusernam.cpp

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh

TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
