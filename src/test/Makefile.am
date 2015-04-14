include $(top_srcdir)/buildutils/config.mk

SERVER_UT_DIRS = accounting array_func array_upgrade attr_recov batch_request completed_jobs_map \
								 delete_all_tracker dis_read display_alps_status execution_slot_tracker \
								 exiting_jobs geteusernam get_path_jobdata id_map incoming_request \
								 issue_request job_attr_def job_container job_func job_qs_upgrade job_recov \
								 job_recycler job_usage_info login_nodes mom_hierarchy_handler node_func \
								 node_manager pbsd_init pbsd_main process_alps_status process_mom_update \
								 process_request queue_func queue_recov queue_recycler receive_mom_communication \
								 reply_send req_delete req_deletearray req_getcred req_gpuctrl req_holdarray \
								 req_holdjob req_jobobit req_locate req_manager req_message req_modify \
								 req_movejob req_quejob req_register req_rerun req_rescq req_runjob req_select \
								 req_shutdown req_signal req_stat req_tokens req_track resc_def_all run_sched \
								 stat_job svr_chk_owner svr_connect svr_format_job svr_func svr_jobfunc svr_mail \
								 svr_movejob svr_recov svr_resccost svr_task track_alps_reservations \
								 user_info 

LIBUTILS_UT_DIRS = u_MXML u_groups u_hash_map_structs u_lock_ctl u_misc u_mom_hierarchy u_mu \
									 u_mutex_mgr u_putenv u_threadpool u_tree u_users u_xml

LIBATTR_UT_DIRS = attr_atomic attr_fn_acl attr_fn_arst attr_fn_b attr_fn_c attr_fn_freq \
									attr_fn_hold attr_fn_intr attr_fn_intr attr_fn_l attr_fn_ll attr_fn_nppcu \
									attr_fn_resc attr_fn_size attr_fn_str attr_fn_time attr_fn_tokens attr_fn_tv \
									attr_fn_unkn attr_func attr_node_func

LIBCMDS_UT_DIRS = add_verify_resources ck_job_name cnt2server cvtdate get_server locate_job \
									parse_at parse_depend parse_destid parse_equal parse_jobid parse_stage \
									prepare_path prt_job_err set_attr set_resource

LIBDIS_UT_DIRS = discui_ discul_ disi10d_ disi10l_ disiui_ disp10d_ disp10l_ disrcs disrd disrf \
								 disrfcs disrfst disrl disrl_ disrsc disrsi disrsi_ disrsl disrsl_ disrss disrst \
								 disruc disrui disrul disrus diswcs diswf diswl_ diswsi diswsl diswui diswui_ diswul

LIBCSV_UT_DIRS = csv


CHECK_DIRS = scaffold_fail torque_test_lib ${SERVER_UT_DIRS} ${LIBUTILS_UT_DIRS} \
						 ${LIBATTR_UT_DIRS} ${LIBCMDS_UT_DIRS} ${LIBDIS_UT_DIRS} ${LIBCSV_UT_DIRS} 

$(CHECK_DIRS)::
	$(MAKE) -C $@ $(MAKECMDGOALS)

check: $(CHECK_DIRS)
