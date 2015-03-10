PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libenc_Shut.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_enc_Shut

libenc_Shut_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/enc_Shut.cpp
libenc_Shut_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_enc_Shut_SOURCES = test_enc_Shut.cpp

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS} 

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
