PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libPBSD_status.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_PBSD_status

libPBSD_status_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/PBSD_status.cpp
libPBSD_status_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_PBSD_status_SOURCES = test_PBSD_status.cpp

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS} 

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
