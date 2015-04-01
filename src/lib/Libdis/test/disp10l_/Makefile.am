PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libdisp10l_.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_disp10l_

libdisp10l__la_SOURCES = scaffolding.cpp ${PROG_ROOT}/disp10l_.cpp
libdisp10l__la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_disp10l__SOURCES = test_disp10l_.cpp

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
