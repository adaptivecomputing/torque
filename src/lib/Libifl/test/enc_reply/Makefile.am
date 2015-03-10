PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libenc_reply.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_enc_reply

libenc_reply_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/enc_reply.cpp
libenc_reply_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_enc_reply_SOURCES = test_enc_reply.cpp

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS} 

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
