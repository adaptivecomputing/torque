PROG_ROOT = ../..

AM_CXXFLAGS = -g -DTEST_FUNCTION -I/usr/include/libxml2 -I${PROG_ROOT}/ --coverage

lib_LTLIBRARIES = libjob_recov.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_job_recov

libjob_recov_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/job_recov.cpp ${PROG_ROOT}/job_func.cpp \
			  ${PROG_ROOT}/svr_func.cpp ${PROG_ROOT}/resc_def_all.cpp ${PROG_ROOT}/req_quejob.cpp \
			  ${PROG_ROOT}/attr_recov.cpp ${PROG_ROOT}/svr_attr_def.cpp ${PROG_ROOT}/job_attr_def.cpp \
			  ${PROG_ROOT}/../lib/Libattr/attr_func.cpp ${PROG_ROOT}/../lib/Libifl/list_link.cpp \
			  ${PROG_ROOT}/../lib/Libattr/attr_fn_resc.cpp ${PROG_ROOT}/../lib/Libattr/attr_fn_arst.cpp \
			  ${PROG_ROOT}/../lib/Libattr/attr_fn_str.cpp ${PROG_ROOT}/../lib/Libattr/attr_fn_c.cpp \
			  ${PROG_ROOT}/../lib/Libattr/attr_fn_hold.cpp ${PROG_ROOT}/../lib/Libattr/attr_fn_tv.cpp \
			  ${PROG_ROOT}/../lib/Libattr/attr_fn_nppcu.cpp \
			  ${PROG_ROOT}/../lib/Libattr/attr_fn_freq.cpp \
			  ${PROG_ROOT}/../lib/Libcsv/csv.cpp \
			  ${PROG_ROOT}/../lib/Liblog/pbs_messages.cpp ${PROG_ROOT}/req_register.cpp
libjob_recov_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_job_recov_SOURCES = test_job_recov.cpp

check_SCRIPTS = ${PROG_ROOT}/../test/coverage_run.sh

TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
