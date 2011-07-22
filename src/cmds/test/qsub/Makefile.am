
PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libqsub_functions.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_x11_get_proto

libqsub_functions_la_SOURCES = scaffolding.c ${PROG_ROOT}/qsub_functions.c
libqsub_functions_la_LDFLAGS = @CHECK_LIBS@

test_x11_get_proto_SOURCES = test_x11_get_proto.c

check_SCRIPTS = build_test_files.sh coverage_run.sh

TESTS = build_test_files.sh ${check_PROGRAMS} coverage_run.sh

coverage_run.sh:
	echo 'cp -p .libs/qsub_functions.gc* . > /dev/null 2>&1' > $@
	echo 'RESULTS=($$(gcov qsub_functions))' >> $@
	echo 'PARSED_RESULT="TOTALCOV -- $${RESULTS[1]}: Lines($${RESULTS[5]})- $${RESULTS[3]}"' >> $@
	echo 'echo -e "\033[40m\033[1;33m$$PARSED_RESULT\033[0m"' >> $@
	chmod +x $@

build_test_files.sh:
	echo 'echo No test files needed yet.' > $@
	echo 'mkdir pbshome' >> $@
	echo 'mkdir submitFilter' >> $@
	chmod +x $@

CLEANFILES = coverage_run.sh build_test_files.sh *.gcno *.gcda *.gcov core *.log
