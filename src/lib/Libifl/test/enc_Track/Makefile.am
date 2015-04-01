PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libenc_Track.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_enc_Track

libenc_Track_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/enc_Track.cpp
libenc_Track_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_enc_Track_SOURCES = test_enc_Track.cpp

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS} 

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
