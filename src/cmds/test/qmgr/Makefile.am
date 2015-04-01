include $(top_srcdir)/buildutils/config.mk
PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libqmgr.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_qmgr

libqmgr_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/qmgr.cpp
libqmgr_la_LDFLAGS = @CHECK_LIBS@ $(PBS_LIBS) $(READLINE_LIBS) -shared -lgcov

test_qmgr_SOURCES = test_qmgr.cpp

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh

TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
