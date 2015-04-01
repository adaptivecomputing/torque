PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ --coverage

lib_LTLIBRARIES = libqueue_recov.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_queue_recov

libqueue_recov_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/queue_recov.cpp ${PROG_ROOT}/../lib/Libutils/u_xml.cpp
libqueue_recov_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_queue_recov_SOURCES = test_queue_recov.cpp

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh

TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
