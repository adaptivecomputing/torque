PROG_ROOT = ../..

AM_CFLAGS = -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage -DPBS_MOM -DDEMUX=\"$(DEMUX_PATH)\"

lib_LTLIBRARIES = libmom_mach.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_mom_mach

libmom_mach_la_SOURCES = scaffolding.c ${PROG_ROOT}/mom_mach.c
libmom_mach_la_LDFLAGS = @CHECK_LIBS@

test_mom_mach_SOURCES = test_mom_mach.c

check_SCRIPTS = coverage_run.sh

TESTS = ${check_PROGRAMS} coverage_run.sh

coverage_run.sh:
	echo 'cp -p .libs/mom_mach.gc* . >/dev/null 2>&1' > $@
	echo 'RESULTS=($$(gcov mom_mach.gcda))' >> $@
	echo 'PARSED_RESULT="TOTALCOV -- $${RESULTS[1]}: Lines($${RESULTS[5]})- $${RESULTS[3]}"' >> $@
	echo 'echo -e "\033[40m\033[1;33m$$PARSED_RESULT\033[0m"' >> $@
	chmod +x $@

CLEANFILES = coverage_run.sh *.gcno *.gcda *.gcov core *.lo
