include $(top_srcdir)/buildutils/config.mk

SERVER_UT_DIRS = accounting array_func array_upgrade attr_recov batch_request delete_all_tracker \
								 dis_read display_alps_status execution_slot_tracker exiting_jobs \
								 geteusernam id_map issue_request job_func job_qs_upgrade job_recov job_recycler \
								 node_func node_manager pbsd_init pbsd_main process_request queue_func \
                 queue_recov reply_send req_delete req_deletearray req_getcred \
                 req_gpuctrl req_holdarray req_holdjob req_jobobit req_locate req_manager req_message \
								 req_modify req_movejob req_quejob req_register req_rerun req_rescq req_runjob req_select \
					       req_shutdown req_signal req_stat req_tokens req_track resc_def_all run_sched stat_job 


CHECK_DIRS = scaffold_fail torque_test_lib ${SERVER_UT_DIRS}

$(CHECK_DIRS)::
	$(MAKE) -C $@ $(MAKECMDGOALS)

check: $(CHECK_DIRS)
