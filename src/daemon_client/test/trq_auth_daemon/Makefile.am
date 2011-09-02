PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libtrq_auth_daemon.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_trq_auth_daemon

libtrq_auth_daemon_la_SOURCES = scaffolding.c ${PROG_ROOT}/trq_auth_daemon.c
libtrq_auth_daemon_la_LDFLAGS = @CHECK_LIBS@

test_trq_auth_daemon_SOURCES = test_trq_auth_daemon.c

check_SCRIPTS = coverage_run.sh

TESTS = ${check_PROGRAMS} coverage_run.sh

coverage_run.sh:
	echo 'cp -p .libs/trq_auth_daemon.gc* . >/dev/null 2>&1' > $@
	echo 'RESULTS=($$(gcov trq_auth_daemon.gcda))' >> $@
	echo 'PARSED_RESULT="TOTALCOV -- $${RESULTS[1]}: Lines($${RESULTS[5]})- $${RESULTS[3]}"' >> $@
	echo 'echo -e "\033[40m\033[1;33m$$PARSED_RESULT\033[0m"' >> $@
	chmod +x $@

CLEANFILES = coverage_run.sh *.gcno *.gcda *.gcov core *.lo
