PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libchk_file_sec.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_chk_file_sec

libchk_file_sec_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/chk_file_sec.cpp
libchk_file_sec_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_chk_file_sec_SOURCES = test_chk_file_sec.cpp

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
