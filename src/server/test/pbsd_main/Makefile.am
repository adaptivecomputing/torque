include $(top_srcdir)/buildutils/config.mk

PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage -DPBS_SERVER_HOME=\"$(PBS_SERVER_HOME)\" -DPBS_ENVIRON=\"$(PBS_ENVIRON)\"

lib_LTLIBRARIES = libpbsd_main.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_pbsd_main

libpbsd_main_la_SOURCES = scaffolding.c ${PROG_ROOT}/pbsd_main.c
libpbsd_main_la_LDFLAGS = @CHECK_LIBS@ -shared -L../../../lib/test/.libs -lscaffolding_lib

test_pbsd_main_SOURCES = test_pbsd_main.c

check_SCRIPTS = coverage_run.sh

TESTS = ${check_PROGRAMS} coverage_run.sh

coverage_run.sh:
	echo 'cp -p .libs/pbsd_main.gc* . >/dev/null 2>&1' > $@
	echo 'RESULTS=($$(gcov pbsd_main.gcda))' >> $@
	echo 'PARSED_RESULT="TOTALCOV -- $${RESULTS[1]}: Lines($${RESULTS[5]})- $${RESULTS[3]}"' >> $@
	echo 'echo -e "\033[40m\033[1;33m$$PARSED_RESULT\033[0m"' >> $@
	chmod +x $@

CLEANFILES = coverage_run.sh *.gcno *.gcda *.gcov core *.lo
