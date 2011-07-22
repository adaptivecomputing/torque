# Root directory of file under test
PROG_ROOT = ../..

#TEST_FUNCTION is used to #ifdef out things that adjust the enviroment for the test run (i.e. main function in file under test, defines that are needed, or undesired, etc)
AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

#The name of the library used for function replacement
lib_LTLIBRARIES = libcatch_child.la

#This defines the ldflags for all the test harness compiles
AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

#These are the binaries built for a check test run
check_PROGRAMS = test_catch_child test_get_node test_init_abort_jobs test_mom_deljob test_obit_reply test_post_epilogue test_preobit_reply test_scan_for_exiting test_exit_mom_job

#Note: {libary_name}_la_SOURCES.
# {library_name} must match the entry in lib_LTLIBRARIES
# la indicates this is a library and lib_LTLIBRARIES should br cross-referenced
#scaffolding & actual unit(file) under test
libcatch_child_la_SOURCES = scaffolding.c ${PROG_ROOT}/catch_child.c
#This overrides the default AM_LDFLAGS defined above, otherwise we get a cyclic compile
libcatch_child_la_LDFLAGS = @CHECK_LIBS@

#Each entry in check_PROGRAMS has a matching _SOURCES
#{name}_SOURCES is a list of the .c files needed for the individual test run

test_catch_child_SOURCES = test_catch_child.c
test_get_node_SOURCES = test_get_node.c
test_init_abort_jobs_SOURCES = test_init_abort_jobs.c
test_mom_deljob_SOURCES = test_mom_deljob.c
test_obit_reply_SOURCES = test_obit_reply.c
test_post_epilogue_SOURCES = test_post_epilogue.c
test_preobit_reply_SOURCES = test_preobit_reply.c
test_scan_for_exiting_SOURCES = test_scan_for_exiting.c
test_exit_mom_job_SOURCES = test_exit_mom_job.c

#A list of scripts that are needed (And are built below)
check_SCRIPTS = build_test_files.sh coverage_run.sh

#A list of tests that make check runs. They are run in the order indicated on the line
TESTS = build_test_files.sh ${check_PROGRAMS} coverage_run.sh

#The contents the coverage run script.
#NOTE: If the script already exists, it will NOT be overwritten. Even if the current contents differ from the contents below.
coverage_run.sh:
	echo 'cp -p .libs/catch_child.gc* . >/dev/null 2>&1' > $@
	echo 'RESULTS=($$(gcov catch_child))' >> $@
	echo 'PARSED_RESULT="TOTALCOV -- $${RESULTS[1]}: Lines($${RESULTS[5]})- $${RESULTS[3]}"' >> $@
	echo 'echo -e "\033[40m\033[1;33m$$PARSED_RESULT\033[0m"' >> $@
	chmod +x $@

#The contents of the scripts used for creation of files needed for the test harness to complete function testing.
build_test_files.sh:
	echo 'touch 1.sysname.ac.JB' > $@
	echo 'touch 2.sysname.ac.JB' >> $@
	echo 'touch 3.sysname.ac.JB' >> $@
	chmod +x $@

#Additional files that would not normally be removed as part of clean that still need to go
CLEANFILES = coverage_run.sh build_test_files.sh *.JB *.gcno *.gcda *.gcov core *.log
