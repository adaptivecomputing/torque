PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libenc_JobObit.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_enc_JobObit

libenc_JobObit_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/enc_JobObit.cpp
libenc_JobObit_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_enc_JobObit_SOURCES = test_enc_JobObit.cpp

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS} 

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
