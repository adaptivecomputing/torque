PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ --coverage

lib_LTLIBRARIES = libreq_locate.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_req_locate

libreq_locate_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/req_locate.cpp
libreq_locate_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_req_locate_SOURCES = test_req_locate.cpp

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh

TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
