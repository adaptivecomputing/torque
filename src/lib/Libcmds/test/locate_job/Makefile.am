PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = liblocate_job.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_locate_job

liblocate_job_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/locate_job.cpp
liblocate_job_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_locate_job_SOURCES = test_locate_job.cpp

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
