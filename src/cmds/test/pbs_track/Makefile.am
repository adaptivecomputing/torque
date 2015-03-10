PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libpbs_track.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_pbs_track

libpbs_track_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/pbs_track.cpp
libpbs_track_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_pbs_track_SOURCES = test_pbs_track.cpp

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh

TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
