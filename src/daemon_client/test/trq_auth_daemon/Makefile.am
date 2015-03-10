PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage -DPBS_SERVER_HOME=\"$(PBS_SERVER_HOME)\"

lib_LTLIBRARIES = libtrq_auth_daemon.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES} ${PROG_ROOT}/../lib/Libpbs/.libs/libtorque.la

check_PROGRAMS = test_trq_auth_daemon

libtrq_auth_daemon_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/trq_auth_daemon.cpp
libtrq_auth_daemon_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_trq_auth_daemon_SOURCES = test_trq_auth_daemon.cpp

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
