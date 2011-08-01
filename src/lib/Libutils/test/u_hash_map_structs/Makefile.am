
PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

CFLAGS = -g -fprofile-arcs -ftest-coverage

TPROGS = ct_u_hash_map_structs

check_PROGRAMS = ${TPROGS}
#check_PROGRAMS = u_hash_map_structs_cov ${TPROGS}

lib_LTLIBRARIES = libu_hash_map_structs.la
noinst_LIB = libu_hash_map_structs.la

BASE_SOURCE = scaffolding.c ${PROG_ROOT}/u_hash_map_structs.c

METHOD_SOURCES = u_hash_map_structs_ct.c

libu_hash_map_structs_la_SOURCES = ${BASE_SOURCE} ${METHOD_SOURCES}

#u_hash_map_structs_cov_SOURCES = ${BASE_SOURCE} ${METHOD_SOURCES} u_hash_map_structs_cov.c
#u_hash_map_structs_cov_LDADD = @CHECK_LIBS@
#u_hash_map_structs_cov_CFLAGS = ${AM_CFLAGS}

ct_u_hash_map_structs_SOURCES = scaffolding.c u_hash_map_structs_ct.c
ct_u_hash_map_structs_LDADD = @CHECK_LIBS@ ${lib_LTLIBRARIES}
ct_u_hash_map_structs_CFLAGS = ${AM_CFLAGS}

check_SCRIPTS = coverage_run.sh

TESTS = ${TPROGS} coverage_run.sh

#	echo './u_hash_map_structs_cov >/dev/null 2>&1' > $@
coverage_run.sh:
	echo 'cp -p .libs/u_hash_map_structs.gc* . >/dev/null 2>&1' > $@
	echo 'RESULTS=($$(gcov u_hash_map_structs.gcda))' >> $@
	echo 'PARSED_RESULT="TOTALCOV -- $${RESULTS[1]}: Lines($${RESULTS[5]})- $${RESULTS[3]}"' >> $@
	echo 'echo -e "\033[40m\033[1;33m$$PARSED_RESULT\033[0m"' >> $@
	chmod +x $@

CLEANFILES = coverage_run.sh *.gcno *.gcda *.gcov core *.log
