PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I/usr/include/libxml2 -I${PROG_ROOT}/ --coverage

lib_LTLIBRARIES = libjob_recov.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_job_recov

libjob_recov_la_SOURCES = scaffolding.c ${PROG_ROOT}/job_recov.c ${PROG_ROOT}/job_func.c \
			  ${PROG_ROOT}/svr_func.c ${PROG_ROOT}/resc_def_all.c ${PROG_ROOT}/req_quejob.c \
			  ${PROG_ROOT}/attr_recov.c ${PROG_ROOT}/svr_attr_def.c ${PROG_ROOT}/job_attr_def.c \
			  ${PROG_ROOT}/../lib/Libattr/attr_func.c ${PROG_ROOT}/../lib/Libifl/list_link.c \
			  ${PROG_ROOT}/../lib/Libattr/attr_fn_resc.c ${PROG_ROOT}/../lib/Libattr/attr_fn_arst.c \
			  ${PROG_ROOT}/../lib/Libattr/attr_fn_str.c ${PROG_ROOT}/../lib/Libattr/attr_fn_c.c \
			  ${PROG_ROOT}/../lib/Libattr/attr_fn_hold.c ${PROG_ROOT}/../lib/Libattr/attr_fn_tv.c \
			  ${PROG_ROOT}/../lib/Libattr/attr_fn_nppcu.c \
			  ${PROG_ROOT}/../lib/Libattr/attr_fn_freq.c \
			  ${PROG_ROOT}/../lib/Libcsv/csv.c \
			  ${PROG_ROOT}/../lib/Liblog/pbs_messages.c ${PROG_ROOT}/req_register.c
libjob_recov_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_job_recov_SOURCES = test_job_recov.c

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh

TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
