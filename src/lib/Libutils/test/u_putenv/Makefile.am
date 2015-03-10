PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libu_putenv.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_u_putenv

libu_putenv_la_SOURCES =  ${PROG_ROOT}/u_putenv.cpp  scaffolding.cpp
libu_putenv_la_LDFLAGS = @CHECK_LIBS@ -lgcov

test_u_putenv_SOURCES = test_u_putenv.cpp 

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
