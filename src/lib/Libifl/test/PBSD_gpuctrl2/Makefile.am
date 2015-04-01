PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libPBSD_gpuctrl2.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_PBSD_gpuctrl2

libPBSD_gpuctrl2_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/PBSD_gpuctrl2.cpp
libPBSD_gpuctrl2_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_PBSD_gpuctrl2_SOURCES = test_PBSD_gpuctrl2.cpp

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS} 

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
