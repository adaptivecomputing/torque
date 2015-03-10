PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libu_mutex_mgr.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}


LIBS+= -lgcov

check_PROGRAMS = test_u_mutex_mgr

libu_mutex_mgr_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/u_mutex_mgr.cpp
libu_mutex_mgr_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_u_mutex_mgr_SOURCES = test_u_mutex_mgr.cpp

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
