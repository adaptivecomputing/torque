# Root directory of file under test
PROG_ROOT = ../..

#TEST_FUNCTION is used to #ifdef out things that adjust the enviroment for the test run (i.e. main function in file under test, defines that are needed, or undesired, etc)
AM_CXXFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage -DPBS_MOM

#The name of the library used for function replacement
lib_LTLIBRARIES = libcatch_child.la

#This defines the ldflags for all the test harness compiles
AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES} -lgcov

#These are the binaries built for a check test run
check_PROGRAMS = test_catch_child test_get_node test_init_abort_jobs test_mom_deljob test_obit_reply test_post_epilogue test_scan_for_exiting test_exit_mom_job

#Note: {libary_name}_la_SOURCES.
# {library_name} must match the entry in lib_LTLIBRARIES
# la indicates this is a library and lib_LTLIBRARIES should br cross-referenced
#scaffolding & actual unit(file) under test
libcatch_child_la_SOURCES = scaffolding.cpp ${PROG_ROOT}/catch_child.cpp
#This overrides the default AM_LDFLAGS defined above, otherwise we get a cyclic compile
libcatch_child_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

#Each entry in check_PROGRAMS has a matching _SOURCES
#{name}_SOURCES is a list of the .cpp files needed for the individual test run

test_catch_child_SOURCES = test_catch_child.cpp
test_get_node_SOURCES = test_get_node.cpp
#test_init_abort_jobs_SOURCES = test_init_abort_jobs.c
test_mom_deljob_SOURCES = test_mom_deljob.cpp
test_obit_reply_SOURCES = test_obit_reply.cpp
#test_post_epilogue_SOURCES = test_post_epilogue.c
test_scan_for_exiting_SOURCES = test_scan_for_exiting.cpp
test_exit_mom_job_SOURCES = test_exit_mom_job.cpp

#A list of tests that make check runs. They are run in the order indicated on the line
check_SCRIPTS = build_test_files.sh ${PROG_ROOT}/../test/coverage_run.sh

TESTS = $(check_PROGRAMS) ${check_SCRIPTS}

#The contents of the scripts used for creation of files needed for the test harness to complete function testing.
build_test_files.sh:
	echo 'touch 1.sysname.ac.JB' > $@
	echo 'touch 2.sysname.ac.JB' >> $@
	echo 'touch 3.sysname.ac.JB' >> $@
	chmod +x $@

#Additional files that would not normally be removed as part of clean that still need to go
CLEANFILES = build_test_files.sh *.JB *.gcno *.gcda *.gcov core *.log
