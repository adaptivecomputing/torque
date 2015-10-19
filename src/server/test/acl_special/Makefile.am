 
PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} -I$(PROG_ROOT)/../include --coverage `xml2-config --cflags`
AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} -I$(PROG_ROOT)/../include --coverage `xml2-config --cflags`
AM_LIBS=`xml2-config --libs`

lib_LTLIBRARIES = libtest_acl_special.la

AM_LDFLAGS = @CHECK_LIBS@ $(lib_LTLIBRARIES) $(AM_LIBS)

check_PROGRAMS = test_acl_special

libtest_acl_special_la_SOURCES = scaffolding.c ${PROG_ROOT}/acl_special.cpp ${PROG_ROOT}/restricted_host.cpp
libtest_acl_special_la_LDFLAGS = @CHECK_LIBS@ ${AM_LIBS} -shared -lgcov

test_acl_special_SOURCES = test_acl_special.c

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh
TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov_core *.lo
