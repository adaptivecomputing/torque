PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ --coverage

lib_LTLIBRARIES = libstat_job.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_stat_job

libstat_job_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/stat_job.cpp
libstat_job_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_stat_job_SOURCES = test_stat_job.cpp

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh

TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
