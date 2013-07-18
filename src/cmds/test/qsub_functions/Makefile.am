
PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libqsub_functions.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_x11_get_proto

libqsub_functions_la_SOURCES = scaffolding.c ${PROG_ROOT}/qsub_functions.c
libqsub_functions_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_x11_get_proto_SOURCES = test_x11_get_proto.c

check_SCRIPTS = build_test_files.sh ${PROG_ROOT}/../test/coverage_run.sh

TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

build_test_files.sh:
	echo 'echo No test files needed yet.' > $@
	echo 'mkdir -p pbshome' >> $@
	echo 'mkdir -p submitFilter' >> $@
	chmod +x $@

CLEANFILES = build_test_files.sh *.gcno *.gcda *.gcov core *.log
