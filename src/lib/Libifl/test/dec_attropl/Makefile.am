PROG_ROOT = ../..

AM_CFLAGS = -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libdec_attropl.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_dec_attropl

libdec_attropl_la_SOURCES = scaffolding.c ${PROG_ROOT}/dec_attropl.c
libdec_attropl_la_LDFLAGS = @CHECK_LIBS@

test_dec_attropl_SOURCES = test_dec_attropl.c

check_SCRIPTS = coverage_run.sh

TESTS = ${check_PROGRAMS} coverage_run.sh

coverage_run.sh:
	echo 'cp -p .libs/dec_attropl.gc* . >/dev/null 2>&1' > $@
	echo 'RESULTS=($$(gcov dec_attropl.gcda))' >> $@
	echo 'PARSED_RESULT="TOTALCOV -- $${RESULTS[1]}: Lines($${RESULTS[5]})- $${RESULTS[3]}"' >> $@
	echo 'echo -e "\033[40m\033[1;33m$$PARSED_RESULT\033[0m"' >> $@
	chmod +x $@

CLEANFILES = coverage_run.sh *.gcno *.gcda *.gcov core *.lo
