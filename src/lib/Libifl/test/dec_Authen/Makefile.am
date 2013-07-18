PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libdec_Authen.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_dec_Authen

libdec_Authen_la_SOURCES = scaffolding.c ${PROG_ROOT}/dec_Authen.c
libdec_Authen_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_dec_Authen_SOURCES = test_dec_Authen.c

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS} 

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
