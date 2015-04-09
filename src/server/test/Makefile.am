PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libscaffolding_svr.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

libscaffolding_svr_la_SOURCES = scaffolding_job_attr_def.c scaffolding_svr.c
libscaffolding_svr_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

CLEANFILES = *.gcno *.gcda *.gcov core *.lo

CHECK_DIRS =  \
					svr_chk_owner svr_connect svr_format_job svr_func svr_jobfunc svr_mail svr_movejob \
					svr_recov svr_resccost svr_task process_alps_status login_nodes \
					track_alps_reservations user_info job_container receive_mom_communication \
					process_mom_update job_attr_def incoming_request \
					job_usage_info mom_hierarchy_handler

$(CHECK_DIRS)::
	$(MAKE) -C $@ $(MAKECMDGOALS)

check: $(lib_LTLIBRARIES) $(CHECK_DIRS)

cleancheck:
	@for dir in $(CHECK_DIRS); do (cd $$dir && $(MAKE) clean); done
