include $(top_srcdir)/buildutils/config.mk

SERVER_UT_DIRS = accounting array_func array_upgrade attr_recov batch_request delete_all_tracker \
								 dis_read display_alps_status execution_slot_tracker exiting_jobs \
								 geteusernam id_map issue_request job_func job_qs_upgrade job_recov job_recycler \
								 node_func node_manager pbsd_init pbsd_main process_request queue_func

CHECK_DIRS = scaffold_fail torque_test_lib ${SERVER_UT_DIRS}

$(CHECK_DIRS)::
	$(MAKE) -C $@ $(MAKECMDGOALS)

check: $(CHECK_DIRS)
