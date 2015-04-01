PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ --coverage

lib_LTLIBRARIES = libsvr_chk_owner.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_svr_chk_owner

libsvr_chk_owner_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/svr_chk_owner.cpp
libsvr_chk_owner_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_svr_chk_owner_SOURCES = test_svr_chk_owner.cpp

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh

TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
