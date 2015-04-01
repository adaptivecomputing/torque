include $(top_srcdir)/buildutils/config.mk

PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ --coverage -DPBS_SERVER_HOME=\"$(PBS_SERVER_HOME)\" -DPBS_ENVIRON=\"$(PBS_ENVIRON)\"

lib_LTLIBRARIES = libpbsd_init.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_pbsd_init

libpbsd_init_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/pbsd_init.cpp
libpbsd_init_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_pbsd_init_LDADD = ../../../test/torque_test_lib/libtorque_test.la ../../../test/scaffold_fail/libscaffold_fail.la
test_pbsd_init_SOURCES = test_pbsd_init.cpp

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh

TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
