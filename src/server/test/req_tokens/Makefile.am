PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ --coverage

lib_LTLIBRARIES = libreq_tokens.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_req_tokens

libreq_tokens_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/req_tokens.cpp
libreq_tokens_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_req_tokens_SOURCES = test_req_tokens.cpp

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh

TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
