PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libu_lock_ctl.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_u_lock_ctl

libu_lock_ctl_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/u_lock_ctl.cpp
libu_lock_ctl_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_u_lock_ctl_SOURCES = test_u_lock_ctl.cpp

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
