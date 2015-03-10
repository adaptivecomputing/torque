PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ --coverage -DPBS_SERVER_HOME=\"$(PBS_SERVER_HOME)\" `xml2-config --cflags`

lib_LTLIBRARIES = libpam_pbssimpleauth.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES} `xml2-config --libs`

check_PROGRAMS = test_pam_pbssimpleauth

libpam_pbssimpleauth_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/pam_pbssimpleauth.cpp
libpam_pbssimpleauth_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov -lpam

test_pam_pbssimpleauth_SOURCES = test_pam_pbssimpleauth.cpp

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh

TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
