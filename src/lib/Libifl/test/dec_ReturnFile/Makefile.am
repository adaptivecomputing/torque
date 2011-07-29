PROG_ROOT = ../..

AM_CFLAGS = -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libdec_ReturnFile.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_dec_ReturnFile

libdec_ReturnFile_la_SOURCES = scaffolding.c ${PROG_ROOT}/dec_ReturnFile.c
libdec_ReturnFile_la_LDFLAGS = @CHECK_LIBS@

test_dec_ReturnFile_SOURCES = test_dec_ReturnFile.c

check_SCRIPTS = coverage_run.sh

TESTS = ${check_PROGRAMS} coverage_run.sh

coverage_run.sh:
	echo 'cp -p .libs/dec_ReturnFile.gc* . >/dev/null 2>&1' > $@
	echo 'RESULTS=($$(gcov dec_ReturnFile.gcda))' >> $@
	echo 'PARSED_RESULT="TOTALCOV -- $${RESULTS[1]}: Lines($${RESULTS[5]})- $${RESULTS[3]}"' >> $@
	echo 'echo -e "\033[40m\033[1;33m$$PARSED_RESULT\033[0m"' >> $@
	chmod +x $@

CLEANFILES = coverage_run.sh *.gcno *.gcda *.gcov core *.lo
