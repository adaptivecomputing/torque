
PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage -lgcov

lib_LTLIBRARIES = libu_hash_map_structs.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}
 
check_PROGRAMS = test_u_hash_map_structs

libu_hash_map_structs_la_SOURCES = scaffolding.c ${PROG_ROOT}/u_hash_map_structs.c
libu_hash_map_structs_la_LDFLAGS = @CHECK_LIBS@ -shared

test_u_hash_map_structs_SOURCES = test_u_hash_map_structs.c

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
