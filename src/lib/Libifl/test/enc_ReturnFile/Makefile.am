PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libenc_ReturnFile.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_enc_ReturnFile

libenc_ReturnFile_la_SOURCES = scaffolding.c ${PROG_ROOT}/enc_ReturnFile.c
libenc_ReturnFile_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_enc_ReturnFile_SOURCES = test_enc_ReturnFile.c

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS} 

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
