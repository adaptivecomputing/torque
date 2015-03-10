PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libPBSD_submit_caps.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_PBSD_submit_caps

libPBSD_submit_caps_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/PBSD_submit_caps.cpp
libPBSD_submit_caps_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_PBSD_submit_caps_SOURCES = test_PBSD_submit_caps.cpp

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS} 

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
