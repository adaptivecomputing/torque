PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libscaffolding_svr.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

libscaffolding_svr_la_SOURCES = scaffolding_job_attr_def.c scaffolding_svr.c
libscaffolding_svr_la_LDFLAGS = @CHECK_LIBS@ -shared

CLEANFILES = *.gcno *.gcda *.gcov core *.lo

SUBDIRS = . accounting array_func array_upgrade attr_recov dis_read geteusernam issue_request job_func \
					job_qs_upgrade job_recov job_recycler job_route node_func node_manager pbsd_init pbsd_main \
					process_request queue_func queue_recov reply_send req_delete req_deletearray req_getcred \
					req_gpuctrl req_holdarray req_holdjob req_jobobit req_locate req_manager req_message \
					req_modify req_movejob req_quejob req_register req_rerun req_rescq req_runjob req_select \
					req_shutdown req_signal req_stat req_tokens req_track resc_def_all run_sched stat_job \
					svr_chk_owner svr_connect svr_format_job svr_func svr_jobfunc svr_mail svr_movejob \
					svr_recov svr_resccost svr_task display_alps_status process_alps_status login_nodes \
					track_alps_reservations user_info exiting_jobs job_container
