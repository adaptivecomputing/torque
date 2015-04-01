include $(top_srcdir)/buildutils/config.mk

PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ --coverage -DPBS_SERVER_HOME=\"$(PBS_SERVER_HOME)\" -DPBS_ENVIRON=\"$(PBS_ENVIRON)\" `xml2-config --cflags`

lib_LTLIBRARIES = libpbsd_main.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_pbsd_main

libpbsd_main_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/pbsd_main.cpp
libpbsd_main_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_pbsd_main_SOURCES = test_pbsd_main.cpp

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh

TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
